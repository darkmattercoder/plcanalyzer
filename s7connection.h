#ifndef S7CONNECTION_H
#define S7CONNECTION_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <QMessageBox>
#include <QDebug>
#include <cstdint>
#include <nodave.h>
#include <openS7online.h>
#include <openSocket.h>

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
    void readSlots(ConSlot cSlot[], int iAmountSlots);
    QString interpret(ConSlot cSlot);

    // Public Variables
    ConSets MyConSet;
};


#endif // S7CONNECTION_H



