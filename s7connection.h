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

class S7Connection
{
private:
    // Private Variables
    _daveOSserialType fds;
    daveInterface * di;
    daveConnection * dc, *dc2;
    QMessageBox msgBox;

    // Private Functions
    typedef int (DECL2 * _setHWnd) (int, HWND);
    _openFunc SCP_open;
    _closeFunc SCP_close;
    _sendFunc SCP_send;
    _receiveFunc SCP_receive;
    _setHWnd SetSinecHWnd;
    _get_errnoFunc SCP_get_errno;
    HANDLE S7Connection::openS7online(const char * accessPoint, HWND handle);
    HANDLE S7Connection::closeS7online(int h);

public:
    // Public Functions
    S7Connection();
    ~S7Connection();
    //openS7online * inopenS7online;

    bool startConnection(HWND WndHandle);
    void diconnect();
    int getValue();

    // Public Variables
    int localMPI;
    int useProto;
    int speed;
    int plcMPI;
    int plc2MPI;
};

#endif // S7CONNECTION_H



