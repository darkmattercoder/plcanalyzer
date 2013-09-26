/******************************************************************************
*   *File: s7connection.cpp                                                   *
*   *Date: 2013-06-01                                                         *
*   *Version: 1.0                                                             *
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

// Class constructor
S7Connection::S7Connection()
{
    // Reserve memory for connection settings
    MyConSet = new ConSets;

    // Initialisize Variables
    MyConSet->localMPI = 0;
    MyConSet->useProto = daveProtoISOTCP;
    MyConSet->speed = daveSpeed187k;
    MyConSet->plcMPI = 2;
    MyConSet->plc2MPI = -1;
    MyConSet->IP_Adr = "0.0.0.0";
    MyConSet->rack = 0;
    MyConSet->slot = 2;
    initSuccess = false;
}

// Class deconstructor
S7Connection::~S7Connection()
{
    // Is an connection established?
    if (isConnected())
    {
        // Disconnect PLC
        disconnect();
    }

    // Free memory
    delete MyConSet;
}

// Opens the Connection witch is set in MyConSets
bool S7Connection::startConnection(WId WndHandle)
{
    // Only connect once
    if(!isConnected())
    {
        // Clear pointer
        fds.rfd = NULL;

        // Get the pointer to the desired entypoint
        switch(MyConSet->useProto)
        {
        case daveProtoMPI:
        case daveProtoPPI:
        case daveProtoAS511:
            std::cout << "Diese Verbinung ist in Vorbereitung" << std::endl;
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
            std::cout << "Abbruch! Keine gueltiges Protokoll gewaehlt!" <<
                         std::endl;
            return false;
            break;
        }

        // Copy entypoint pointer
        fds.wfd=fds.rfd;

        // Is there a valid pointer
        if (((int)fds.rfd)>=0)  	// had Problems here: HANDLE is unsigned,
        {                           //0 is a valid HANDLE for s7onlinx
            // Open new interface
            di = daveNewInterface(fds, "IF1", MyConSet->localMPI, MyConSet->
                                  useProto, MyConSet->speed);

            // Set Timout for interface
            daveSetTimeout(di, 1000000);


            // If S7 Online Protocol -> initialize adapter
            if (MyConSet->useProto == daveProtoS7online)
            {
                char buf1 [davePartnerListSize];
                int iAmountPartners;
                for (int i=0; i<3; i++)
                {
                    // Initalize Adapter
                    if (0==daveInitAdapter(di))
                    {
                        initSuccess = true;
                        iAmountPartners = daveListReachablePartners(di, buf1);
                        std::cout << "daveListReachablePartners List length:  "
                                  << iAmountPartners << std::endl;
                        if(iAmountPartners > 0)
                        {
                            for (int j = 0; j < iAmountPartners; j++)
                            {
                                if (buf1[j] == daveMPIReachable)
                                {
                                    std::cout << "Aktiver Teilnehmer mit"
                                                 "Adresse: " << j <<
                                                 std::endl;
                                }
                                if (buf1[j] == daveMPIPassive)
                                {
                                    std::cout << "Passiver Teilnehmer mit"
                                                 "Adresse: " << j <<
                                                 std::endl;
                                }
                            }
                        }
                        break;
                    }
                    else
                    {
                        // Could not initalize Adapter
                        // Free the interface
                        daveDisconnectAdapter(di);
                    }

                }

                // If adapter couldn´t be initalised -> Error Message
                if (!initSuccess)
                {
                    std::cout << "Konnte keine Verbindung mit dem Adapter"
                                 "herstellen! Bitte versuchen sie es"
                                 "nocheinmal." << std::endl;
                }
            }

            // Open a new connection
            dc = daveNewConnection(di, MyConSet->plcMPI, MyConSet->rack,
                                   MyConSet->slot);

            // Use 2nd Method of MPI Connection
            if(MyConSet->plc2MPI>=0)
            {
                dc2 =daveNewConnection(di,MyConSet->plc2MPI,0,0);
            }
            else
            {
                dc2=NULL;
            }

            // Try to connect with opened connection
            if (0==daveConnectPLC(dc))
            {
                std::cout << "Verbindung erfolgreich hergestellt." << std::endl;
                initSuccess = true;

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
                std::cout << "Es konnte keine Verbinung aufgebaut werden."
                          << std::endl;
                daveDisconnectAdapter(di);

                // Close the right entrypoint
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
                    // Unknown protocol type choosen
                    std::cout << "Unbekannter Protokolltyp!" << std::endl;
                    return false;
                    break;
                }
            }
        }
        else
        {
            // Couldn't open access point
            std::cout << "Zugangspunkt konnte nicht geoeffnet werden" << std::endl;
        }
    }
    else
    {
        // Connection already exists
        std::cout << "Es besteht bereits eine offene Verbindung" << std::endl;
    }
    return isConnected();
}

// Disconnect from PLC, and frees all reserved memory
void S7Connection::disconnect()
{
    // Just disconnect if connection has existed
    if (isConnected())
    {
        // Disconnect and free variables
        daveDisconnectPLC(dc);
        daveFree(dc);

        // If 2nd Method is choosen -> close it
        if(dc2)
        {
            // Free memory
            daveDisconnectPLC(dc2);
            daveFree(dc);
        }

        // Diconnect opened adapter
        daveDisconnectAdapter(di);

        // Close the opened entypoint
        switch(MyConSet->useProto)
        {
        case daveProtoMPI:
        case daveProtoPPI:
        case daveProtoAS511:
            std::cout << "Diese Verbindung ist noch nicht implementiert" << std::endl;
            break;
#ifdef BCCWIN
        case daveProtoS7online:
            // Close S7Online
            closeS7online(int(fds.rfd));
            break;
#endif
        case daveProtoISOTCP:
        case daveProtoISOTCP243:
        case daveProtoISOTCPR:
            // Close Netsockets
            closeSocket(fds.rfd);
            break;
        default:
            // Unbekannter Protokolltyp gewählt
            std::cout << "Unbekannter Protokolltyp!" << std::endl;
            break;
        }

        // Free interface and set disconnected status
        daveFree(di);

        // Set state disconnected
        initSuccess = false;

        std::cout << "Verbindung wurde getrennt." << std::endl;
    }
}

//Currently only a certain operand is read. To be thrown away or rewritten!
int S7Connection::getValue()
{
    // daveReadBytes(Connection, Adress, DB, Start, Bytes, Buffer)
    int iReturn = 0;
    int res;
    res = daveReadBytes(dc, daveFlags, 0, 1, 1, NULL);

    if (res==0)
    {
        iReturn = daveGetS8(dc);
    }
    return iReturn;
}

// Returns the connection status
bool S7Connection::isConnected()
{
    return initSuccess;
}

// Reads all variables handed in the Slotvector and writes them back
void S7Connection::readSlots(QVector<ConSlot> &cSlot, int iAmountSlots)
{
    PDU p;


    // Prepare the read request
    davePrepareReadRequest(dc, &p);

    // Insert all available slots to read request
    for (int i = 0; i < iAmountSlots; i++)
    {
        // Add Variables to Request
        if (cSlot[i].iDatenlaenge != DatLenBit)
        {

            // Read Byte(s)
            daveAddVarToReadRequest(&p, cSlot[i].iAdrBereich,
                                    cSlot[i].iDBnummer, cSlot[i].iStartAdr,
                                    cSlot[i].iDatenlaenge / 8);
        }
        else
        {
            // Read Bit
            // The bit beginning with addresse 0.0
            int iBit = cSlot[i].iStartAdr * 8 + cSlot[i].iBitnummer;
            daveAddBitVarToReadRequest(&p, cSlot[i].iAdrBereich,
                                       cSlot[i].iDBnummer, iBit , 1);
        }
    }

    daveResultSet rs;
    unsigned long res;

    // Execute read request
    res=daveExecReadRequest(dc, &p, &rs);

    // Write requested data back to the slots
    for (int i = 0; i < iAmountSlots; i++)
    {
        // Get data from resultset
        res=daveUseResult(dc, &rs, i);
        if (res==0)
        {
            switch(cSlot[i].iDatenlaenge)
            {
            case DatLenBit:
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
                std::cout << "Länge muss in Bit angegeben werden und kann"
                             "maximal 32 betragen." << std::endl;
                break;
            }
        }
        else
        {
            // Errorcode output
            std::cout << "*** Error: " << daveStrerror(res) << std::endl;
        }
    }

    // Free result memory
    daveFreeResults(&rs);
}

// Value output in the desired format from handed slot
QString S7Connection::interpret(ConSlot &cSlot)
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
        std::cout << "Laenge muss in Bit angegeben werden und kann maximal 32"
                     "betragen." << std::endl;
        break;
    }
    return szRetVal;
}

// Value output just in decimal oder or float format for graph output
QString S7Connection::interpret(ConSlot &cSlot, bool bGraphOutput)
{
    QString szRetVal;

    if(bGraphOutput)
    {
        switch(cSlot.iDatenlaenge)
        {
        case DatLenBit:
            szRetVal  = QString::number(cSlot.RetVal.Bit);
            break;

        case DatLenByte:
            szRetVal  = QString::number(cSlot.RetVal.Byte);
            break;

        case DatLenWord:
            szRetVal  = QString::number(cSlot.RetVal.Int);

        case DatLenDWord:
            switch(cSlot.iAnzFormat)
            {
            case AnzFormatGleitpunkt:
                szRetVal = QString::number(cSlot.RetVal.Real);
                break;
            default:
                szRetVal  = QString::number(cSlot.RetVal.DInt);
                break;
            }
            break;

        default:
            std::cout << "Laenge muss in Bit angegeben werden und kann maximal 32"
                         "betragen." << std::endl;
            szRetVal = "0";
            break;
        }
        return szRetVal;
    }
    else
    {
        // Execute for normal output
        szRetVal = interpret(cSlot);
    }
    return szRetVal;
}

