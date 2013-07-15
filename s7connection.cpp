/*******************************************************************************
 *   *File: s7connection.cpp                                                   *
 *   *Date: 2013-06-01                                                         *
 *   *Author(s): Jochen Bauer <devel@jochenbauer.net>                          *
 *               Lukas Kern <lukas.kern@online.de>                             *
 *               Carsten Klein <hook-the-master@gmx.net>                       *
 *                                                                             *
 *   *License information:                                                     *
 *                                                                             *
 *   Copyright (C) [2013] [Jochen Bauer, Lukas Kern, Carsten Klein]            *
 *                                                                             *
 *   This file is part of PLCANALYZER. PLCANALYZER is free software; you can   *
 *   redistribute it and/or modify it under the terms of the GNU General       *
 *   Public License as published by the Free Software Foundation; either       *
 *   version 2 of the License, or (at your option) any later version.          *
 *                                                                             *
 *   This program is distributed in the hope that it will be useful, but       *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *   GNU General Public License for more details.                              *
 *   You should have received a copy of the GNU General Public License         *
 *   along with this program; if not, write to the Free Software               *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA             *
 *   02110-1301, USA.                                                          *
 *                                                                             *
 *   *Program name: PLCANALYZER                                                *
 *   *What this program does:    Connects to most SIMATIC S7/S5 Controllers    *
 *                               * Reads memory areas                          *
 *                               * Draws a graph over time for operands        *
 *   *Have fun!                                                                *
 ******************************************************************************/

#include "s7connection.h"
#include <iostream>

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
            qDebug("Diese Verbinung ist in Vorbereitung");
            return false;
            break;
#ifdef BCCWIN
        case daveProtoS7online:
            fds.rfd = openS7online("/S7ONLINE", WndHandle);
            std::cout << "OpenS7online Handle:" << fds.rfd << std::endl;
            break;
#endif
        case daveProtoISOTCP:
        case daveProtoISOTCP243:
        case daveProtoISOTCPR:
            fds.rfd=openSocket(102, MyConSet->IP_Adr.toUtf8());
            break;
        default:
            std::cout << "Abbruch!\nKeine gueltiges Protokoll gewaehlt!" << std::endl;
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
                        std::cout << "daveListReachablePartners List length: %d\n" << a << std::endl;
                        if (a>0) {
                            for (int j=0;j<a;j++) {
                                if (buf1[j]==daveMPIReachable) std::cout << "Aktiver Teilnehmer mit Adresse:%d\n" << j << std::endl;
                                if (buf1[j]==daveMPIPassive) std::cout << "Passiver Teilnehmer mit Adresse:%d\n" << j << std::endl;
                            }
                        }
                        break;
                    } else daveDisconnectAdapter(di);

                }

                if (!initSuccess)
                {
                    std::cout << "Konnte keine Verbindung mit dem Adapter herstellen!\n Bitte versuchen sie es nocheinmal." << std::endl;
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
                std::cout << "Verbindung erfolgreich hergestellt." << std::endl;
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
                std::cout << "Es konnte keine Verbinung aufgebaut werden.\n" << std::endl;
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
                    std::cout << "Unbekannter Protokolltyp!" << std::endl;
                    return false;
                    break;
                }
            }
        } else {
            std::cout << "Couldn't open access point S7ONLINE." << std::endl;
            //return -1;
        }
    }
    else
    {
        std::cout << "Connection already exists!" << std::endl;
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
            std::cout << "Unbekannter Protokolltyp!" << std::endl;
            break;
        }

        daveFree(di);
        initSuccess = 0;

        std::cout << "Verbindung wurde getrennt." << std::endl;
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
                std::cout << "Länge muss in Bit angegeben werden und kann maximal 32 betragen." << std::endl;
                break;
            }
        }
        else
        {
            // Ausgabe des Fehlercodes
            std::cout << "*** Error: %s\n" << daveStrerror(res) << std::endl;
        }
    }

    // Bereinigung des Ergebnisspeichers
    daveFreeResults(&rs);
}

// Wert im gewünschten Format ausgeben
QString S7Connection::interpret(ConSlot cSlot)
{
    QString szRetVal("ERROR");
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
        std::cout << "Länge muss in Bit angegeben werden und kann maximal 32 betragen." << std::endl;
        break;
    }
    return szRetVal;
}
