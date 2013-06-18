#ifndef S7CONNECTION_H
#define S7CONNECTION_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <QMessageBox>
#include <QLibrary>
#include <QFile>
#include <QDir>
#include "nodave.h"
#include "openS7online.h"

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

struct ConSets
{
    int localMPI;
    int useProto;
    int speed;
    int plcMPI;
    int plc2MPI;
    QString IP_Adr;
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
    bool startConnection(HWND WndHandle);
    void disconnect();
    int getValue();
    bool isConnected();

    // Public Variables
//    int localMPI;
//    int useProto;
//    int speed;
//    int plcMPI;
//    int plc2MPI;
    ConSets MyConSet;
};


#endif // S7CONNECTION_H



