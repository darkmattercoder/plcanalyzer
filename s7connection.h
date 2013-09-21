/*******************************************************************************
 *   *File: s7connection.h                                                     *
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

#ifndef S7CONNECTION_H
#define S7CONNECTION_H
#include <QMessageBox>
#include <QDebug>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdint>

#ifdef LINUX
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#define UNIX_STYLE
#endif

#ifdef BCCWIN
#include <windows.h>
#include <time.h>
#include <windows.h>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#define WIN_STYLE
#endif

#include <nodave.h>
#include <openS7online.h>
#include <openSocket.h>

// Verfügbare Anzeigeformate
#define AnzFormatBinaer 1
#define AnzFormatBool 2
#define AnzFormatDezimal 3
#define AnzFormatHexadezimal 4
#define AnzFormatGleitpunkt 5
#define AnzFormatZeichen 6

// Datenlänge in Bit
#define DatLenBit 1
#define DatLenByte 8
#define DatLenWord 16
#define DatLenDWord 32

// Connection Settings
struct ConSets
{
    int localMPI;
    int useProto;
    int speed;
    int plcMPI;
    int plc2MPI;
    int rack;
    int slot;
    QString IP_Adr;
};

// Union Datentypen
union OutputTypes
{
    bool Bit;
    char Byte;
    int16_t Int;
    int32_t DInt;
    float Real;
};

// Connection Slot
struct ConSlot
{
    int iAdrBereich;
    int iDBnummer;
    int iStartAdr;
    int iBitnummer;
    int iDatenlaenge;
    int iAnzFormat;
    QColor graphColor;
    OutputTypes RetVal;
};

class S7Connection
{
private:
    // Private Variables
    _daveOSserialType fds;
    daveInterface * di;
    daveConnection * dc, *dc2;
    QMessageBox msgBox;
    int initSuccess;

    // Private Functions


public:
    // Public Functions
    S7Connection();
    ~S7Connection();

    bool startConnection(WId WndHandle);
    void disconnect();
    int getValue();
    bool isConnected();
    void readSlots(QVector<ConSlot> &cSlot, int iAmountSlots);
//    void readSlots(ConSlot cSlot[], int iAmountSlots);
    QString interpret(ConSlot cSlot);
    QString interpret(ConSlot cSlot, bool bGraphOutput);

    // Public Variables
    ConSets* MyConSet;
};


#endif // S7CONNECTION_H



