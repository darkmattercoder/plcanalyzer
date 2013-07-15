#include "s7connection.h"
#include <iostream>
using namespace std;
// Konstruktor
S7Connection::S7Connection()
{

    MyConSet = new ConSets;

    // Initialisize Variables
    MyConSet->localMPI = 0;
    MyConSet->useProto = daveProtoISOTCP;
    MyConSet->speed = daveSpeed187k;
    MyConSet->plcMPI = 2;
    MyConSet->plc2MPI = -1;
    MyConSet->IP_Adr = "192.168.40.77";
    MyConSet->rack = 0;
    MyConSet->slot = 2;
    initSuccess = 0;
}

// Destructor
S7Connection::~S7Connection()
{
    // Disconnect from PLC
    if (isConnected())
    {
        disconnect();
    }
    delete MyConSet;
}

// Opens the Connection

bool S7Connection::startConnection(WId WndHandle)
{
    if(!isConnected())
    {
        fds.rfd = NULL;

        switch(MyConSet->useProto)
        {
        case daveProtoMPI:
        case daveProtoPPI:
        case daveProtoAS511:

            break;
#ifdef BCCWIN
        case daveProtoS7online:
            fds.rfd = openS7online("/S7ONLINE", WndHandle);
            cout << "OpenS7online Handle:" << fds.rfd << endl;
            break;
#endif
        case daveProtoISOTCP:
        case daveProtoISOTCP243:
        case daveProtoISOTCPR:
            fds.rfd=openSocket(102, MyConSet->IP_Adr.toUtf8());
            break;
        default:
            cout << "Abbruch!\nKeine gueltiges Protokoll gewaehlt!" << endl;
            return false;
            break;
        }



        fds.wfd=fds.rfd;
        if (((int)fds.rfd)>=0) { 	// had Problems here: HANDLE is unsigned, 0 is a valid HANDLE for s7onlinx

            //daveSetDebug(daveDebugListReachables);

            di = daveNewInterface(fds, "IF1", MyConSet->localMPI, MyConSet->useProto, MyConSet->speed);

            daveSetTimeout(di,1000000);

            char buf1 [davePartnerListSize];

            if (MyConSet->useProto == daveProtoS7online)
            {
                int a;
                for (int i=0; i<3; i++) {
                    if (0==daveInitAdapter(di)) {
                        initSuccess=1;
                        a= daveListReachablePartners(di,buf1);
                        cout << "daveListReachablePartners List length: %d\n" << a << endl;
                        if (a>0) {
                            for (int j=0;j<a;j++) {
                                if (buf1[j]==daveMPIReachable) cout << "Aktiver Teilnehmer mit Adresse:%d\n" << j << endl;
                                if (buf1[j]==daveMPIPassive) cout << "Passiver Teilnehmer mit Adresse:%d\n" << j << endl;
                            }
                        }
                        break;
                    } else daveDisconnectAdapter(di);

                }

                if (!initSuccess)
                {
                    cout << "Konnte keine Verbindung mit dem Adapter herstellen!\n Bitte versuchen sie es nocheinmal." << endl;
                }
            }

            // Try to Connect
            dc = daveNewConnection(di, MyConSet->plcMPI, MyConSet->rack, MyConSet->slot);

            if(MyConSet->plc2MPI>=0)
                dc2 =daveNewConnection(di,MyConSet->plc2MPI,0,0);
            else
                dc2=NULL;


            if (0==daveConnectPLC(dc))
            {
                cout << "Verbindung erfolgreich hergestellt." << endl;
                initSuccess=1;

                if(MyConSet->plc2MPI>=0)
                {
                    daveConnectPLC(dc2);
                }
                else
                {
                    dc2=NULL;
                }
            }
            else
            {
                cout << "Es konnte keine Verbinung aufgebaut werden.\n" << endl;
                daveDisconnectAdapter(di);

                switch(MyConSet->useProto)
                {
                case daveProtoMPI:
                case daveProtoPPI:
                case daveProtoAS511:

                    break;
#ifdef BCCWIN
                case daveProtoS7online:
                    closeS7online(int(fds.rfd));
                    break;
#endif
                case daveProtoISOTCP:
                case daveProtoISOTCP243:
                case daveProtoISOTCPR:
                    closeSocket(fds.rfd);
                    break;
                default:
                    // Unbekannter Protokolltyp gewählt
                    cout << "Unbekannter Protokolltyp!" << endl;
                    return false;
                    break;
                }


            }

        } else {
            cout << "Couldn't open access point S7ONLINE." << endl;
            //return -1;
        }
    }
    else
    {
        cout << "Connection already exists!" << endl;

    }
    return isConnected();
}

// Disconnect from PLC
void S7Connection::disconnect()
{
    if (isConnected())
    {
        daveDisconnectPLC(dc);
        daveFree(dc);

        if(dc2)
        {
            daveDisconnectPLC(dc2);
            daveFree(dc);
        }

        daveDisconnectAdapter(di);

        switch(MyConSet->useProto)
        {
        case daveProtoMPI:
        case daveProtoPPI:
        case daveProtoAS511:

            break;
#ifdef BCCWIN
        case daveProtoS7online:
            closeS7online(int(fds.rfd));
            break;
#endif
        case daveProtoISOTCP:
        case daveProtoISOTCP243:
        case daveProtoISOTCPR:
            closeSocket(fds.rfd);
            break;
        default:
            // Unbekannter Protokolltyp gewählt
            cout << "Unbekannter Protokolltyp!" << endl;
            break;
        }

        daveFree(di);
        initSuccess = 0;

        cout << "Verbindung wurde getrennt." << endl;
    }
}



int S7Connection::getValue()
{
    // daveReadBytes(Connection, Adress, DB, Start, Bytes, Buffer)
    int iReturn = 0;
    int res;
    res = daveReadBytes(dc, daveFlags, 0, 1, 1, NULL);

    if (res==0) {
        iReturn = daveGetS8(dc);
    }
    return iReturn;
}

bool S7Connection::isConnected()
{
    bool bRet = false;
    if (initSuccess) bRet = true;
    return bRet;
}

void S7Connection::readSlots(ConSlot cSlot[], int iAmountSlots)
{
    PDU p;
    davePrepareReadRequest(dc, &p);

    for (int i = 0; i < iAmountSlots; i++)
    {
        // Add Variables to Request
        if (cSlot[i].iDatenlaenge != DatLenBit)
        {
            // Read Byte(s)
            daveAddVarToReadRequest(&p, cSlot[i].iAdrBereich, cSlot[i].iDBnummer, cSlot[i].iStartAdr, cSlot[i].iDatenlaenge / 8);
        }
        else
        {
            // Read Bit
            // Das Bit ab Adresse 0.0
            int iBit = cSlot[i].iStartAdr * 8 + cSlot[i].iBitnummer;
            daveAddBitVarToReadRequest(&p, cSlot[i].iAdrBereich, cSlot[i].iDBnummer, iBit , 1);
        }
    }

    daveResultSet rs;
    unsigned long res;
    res=daveExecReadRequest(dc, &p, &rs);

    for (int i = 0; i < iAmountSlots; i++)
    {
        // Use the result
        res=daveUseResult(dc, &rs, i);
        if (res==0)
        {
            switch(cSlot[i].iDatenlaenge)
            {
            case DatLenBit:
                // Read Value
                cSlot[i].RetVal.Bit = bool (daveGetU8(dc));
                break;
            case DatLenByte:
                cSlot[i].RetVal.Byte = char (daveGetU8(dc));
                break;
            case DatLenWord:
                cSlot[i].RetVal.Int = daveGetU16(dc);
                break;
            case DatLenDWord:
                cSlot[i].RetVal.DInt = daveGetU32(dc);
                break;
            default:
                cout << "Länge muss in Bit angegeben werden und kann maximal 32 betragen." << endl;
                break;
            }
        }
        else
        {
            // Ausgabe des Fehlercodes
            cout << "*** Error: %s\n" << daveStrerror(res) << endl;
        }
    }

    // Bereinigung des Ergebnisspeichers
    daveFreeResults(&rs);
}

// Wert im gewünschten Format ausgeben
QString S7Connection::interpret(ConSlot cSlot)
{
    QString szRetVal;
    switch(cSlot.iDatenlaenge)
    {
    case DatLenBit:
           switch(cSlot.iAnzFormat)
        {
        case AnzFormatBinaer:
            szRetVal  = QString::number(cSlot.RetVal.Bit, 2).right(DatLenBit);
            break;
        case AnzFormatBool:
            if (cSlot.RetVal.Bit)
            {
                szRetVal  = "true";
            }
            else
            {
                szRetVal  = "false";
            }
            break;
        case AnzFormatDezimal:
            szRetVal  = QString::number(cSlot.RetVal.Bit);
            break;
        default:
            szRetVal  = "Datentyp kann nicht angezeigt werden";
            break;
        }
        break;

    case DatLenByte:
        switch(cSlot.iAnzFormat)
        {
        case AnzFormatBinaer:
            szRetVal  = QString::number(cSlot.RetVal.Byte, 2).right(DatLenByte);
            break;
        case AnzFormatDezimal:
            szRetVal  = QString::number(cSlot.RetVal.Byte);
            break;
        case AnzFormatHexadezimal:
            szRetVal = QString::number(cSlot.RetVal.Byte, 16.).toUpper();
            break;
        case AnzFormatZeichen:
            szRetVal = QString(cSlot.RetVal.Byte);
            break;
        default:
            szRetVal  = "Datentyp kann nicht angezeigt werden";
            break;
        }
        break;

    case DatLenWord:
        switch(cSlot.iAnzFormat)
        {
        case AnzFormatBinaer:
            szRetVal  = QString::number(cSlot.RetVal.Int, 2).right(DatLenWord);
            break;
        case AnzFormatDezimal:
            szRetVal  = QString::number(cSlot.RetVal.Int);
            break;
        case AnzFormatHexadezimal:
            szRetVal = QString::number(cSlot.RetVal.Int, 16).toUpper();
            break;
        case AnzFormatZeichen:
            szRetVal = QString::number(cSlot.RetVal.Int).toAscii();
            break;
        default:
            szRetVal  = "Datentyp kann nicht angezeigt werden";
            break;
        }
        break;

    case DatLenDWord:
        switch(cSlot.iAnzFormat)
        {
        case AnzFormatBinaer:
            szRetVal  = QString::number(cSlot.RetVal.DInt, 2).right(DatLenDWord);
            break;
        case AnzFormatDezimal:
            szRetVal  = QString::number(cSlot.RetVal.DInt);
            break;
        case AnzFormatHexadezimal:
            szRetVal = QString::number(cSlot.RetVal.DInt).toAscii().toHex();
            break;
        case AnzFormatZeichen:
            szRetVal = QString::number(cSlot.RetVal.DInt).toAscii();
            break;
        case AnzFormatGleitpunkt:
            szRetVal = QString::number(cSlot.RetVal.Real);
            break;
        default:
            szRetVal  = "Datentyp kann nicht angezeigt werden";
            break;
        }
        break;

    default:
        cout << "Länge muss in Bit angegeben werden und kann maximal 32 betragen." << endl;
        break;
    }
    return szRetVal;
}
