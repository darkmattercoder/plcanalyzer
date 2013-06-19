#include "s7connection.h"
#include "log2.h"

// Konstruktor
S7Connection::S7Connection()
{
    // Initialisize Variables
    MyConSet.localMPI = 0;
    MyConSet.useProto = daveProtoISOTCP;
    MyConSet.speed = daveSpeed187k;
    MyConSet.plcMPI = 2;
    MyConSet.plc2MPI = -1;
    MyConSet.IP_Adr = "192.168.40.77";
    MyConSet.rack = 0;
    MyConSet.slot = 2;
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
}

// Opens the Connection
bool S7Connection::startConnection(HWND WndHandle)
{
    if(!isConnected())
    {
        fds.rfd = NULL;

        switch(MyConSet.useProto)
        {
        case daveProtoMPI:
        case daveProtoPPI:
        case daveProtoAS511:

            break;
        case daveProtoS7online:
            fds.rfd = openS7online("/S7ONLINE", WndHandle);
            LOG2("OpenS7online Handle:",(int) fds.rfd);
            FLUSH;
            break;
        case daveProtoISOTCP:
        case daveProtoISOTCP243:
        case daveProtoISOTCPR:
            fds.rfd=openSocket(102, MyConSet.IP_Adr.toUtf8());
            break;
        default:
            printf("Abbruch!\nKeine gueltiges Protokoll gewaehlt!");
            return false;
            break;
        }



        fds.wfd=fds.rfd;
        if (((int)fds.rfd)>=0) { 	// had Problems here: HANDLE is unsigned, 0 is a valid HANDLE for s7onlinx

            //daveSetDebug(daveDebugListReachables);

            di = daveNewInterface(fds, "IF1", MyConSet.localMPI, MyConSet.useProto, MyConSet.speed);

            daveSetTimeout(di,5000000);



            char buf1 [davePartnerListSize];

            if (MyConSet.useProto == daveProtoS7online)
            {
                int a;
                for (int i=0; i<3; i++) {
                    if (0==daveInitAdapter(di)) {
                        initSuccess=1;
                        a= daveListReachablePartners(di,buf1);
                        printf("daveListReachablePartners List length: %d\n",a);
                        if (a>0) {
                            for (int j=0;j<a;j++) {
                                if (buf1[j]==daveMPIReachable) printf("Aktiver Teilnehmer mit Adresse:%d\n",j);
                                if (buf1[j]==daveMPIPassive) printf("Passiver Teilnehmer mit Adresse:%d\n",j);
                            }
                        }
                        break;
                    } else daveDisconnectAdapter(di);

                }



                if (!initSuccess)
                {
                    printf("Konnte keine Verbindung mit dem Adapter herstellen!\n Bitte versuchen sie es nocheinmal.\n");
                }
            }

            // Try to Connect
            dc = daveNewConnection(di, MyConSet.plcMPI, MyConSet.rack, MyConSet.slot);

            if(MyConSet.plc2MPI>=0)
                dc2 =daveNewConnection(di,MyConSet.plc2MPI,0,0);
            else
                dc2=NULL;


            if (0==daveConnectPLC(dc))
            {
                printf("Verbindung erfolgreich hergestellt.\n");
                initSuccess=1;

                if(MyConSet.plc2MPI>=0)
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
                printf("Es konnte keine Verbinung aufgebaut werden.\n");
                daveDisconnectAdapter(di);

                switch(MyConSet.useProto)
                {
                case daveProtoMPI:
                case daveProtoPPI:
                case daveProtoAS511:

                    break;
                case daveProtoS7online:
                    closeS7online(int(fds.rfd));
                    break;
                case daveProtoISOTCP:
                case daveProtoISOTCP243:
                case daveProtoISOTCPR:
                    closeSocket(fds.rfd);
                    break;
                default:
                    // Unbekannter Protokolltyp gewählt
                    printf("Unbekannter Protokolltyp!\n");
                    return false;
                    break;
                }


            }

        } else {
            printf("Couldn't open access point S7ONLINE.\n");
            //return -1;
        }
    }
    else
    {
        printf("Connection already exists!\n");

    }

    FLUSH;
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

        switch(MyConSet.useProto)
        {
        case daveProtoMPI:
        case daveProtoPPI:
        case daveProtoAS511:

            break;
        case daveProtoS7online:
            closeS7online(int(fds.rfd));
            break;
        case daveProtoISOTCP:
        case daveProtoISOTCP243:
        case daveProtoISOTCPR:
            closeSocket(fds.rfd);
            break;
        default:
            // Unbekannter Protokolltyp gewählt
            printf("Unbekannter Protokolltyp!\n");
            break;
        }

        daveFree(di);
        initSuccess = 0;

        printf("Verbindung wurde getrennt.\n");
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
