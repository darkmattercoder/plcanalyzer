#ifndef S7CONNECTION_H
#define S7CONNECTION_H

#include "openS7online.h"
#include "nodave.h"
#include <QtGui/QApplication>

class S7Connection
{
public:
    // Public Functions
    S7Connection();
    //~S7Connection();
    bool connect();
    void diconnect();
    int getValue();

    // Public Variables
    int localMPI;
    int useProto;
    int speed;
    int plcMPI;
    int plc2MPI;

private:
    // Private Variables
    _daveOSserialType fds;
    daveInterface * di;
    daveConnection * dc, *dc2;
};

#endif // S7CONNECTION_H
