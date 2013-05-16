#include "s7connection.h"

// Konstructor
S7Connection::S7Connection()
{
    // Initialisize Variables
    localMPI = 0;
    useProto = daveProtoS7online;
    speed = daveSpeed187k;
    plcMPI = 2;
    plc2MPI = -1;
}
/*
// Destructor
void S7Connection::~S7Connection()
{
    daveDisconnectAdapter(di);
    //closeS7online(fds.rfd);
    daveFree(di);
}
*/
// Opens the Connection
bool S7Connection::connect()
{
    bool bReturn = false;
    //fds.rfd = openS7online("/S7online", MyHWND);
    //fds.wfd=fds.rfd;
    if (((int)fds.rfd)>=0) { 	// had Problems here: HANDLE is unsigned, 0 is a valid HANDLE for s7onlinx

    di = daveNewInterface(fds, "IF1", localMPI, useProto, speed);

    int initSuccess = 0;
    char buf1 [davePartnerListSize];

    daveSetTimeout(di,5000000);

    for (int i=0; i<3; i++) {
        if (0==daveInitAdapter(di)) {
        initSuccess=1;
        int a = daveListReachablePartners(di,buf1);
        printf("daveListReachablePartners List length: %d\n",a);
        if (a>0) {
            for (int j=0;j<a;j++) {
            if (buf1[j]==daveMPIReachable) printf("Active device at address:%d\n",j);
            if (buf1[j]==daveMPIPassive) printf("Passive device at address:%d\n",j);
            }
        }
        break;
        } else daveDisconnectAdapter(di);

    }

    if (!initSuccess) {
        printf("Couldn't connect to Adapter!.\n Please try again. You may also try the option -2 for some adapters.\n");
    } else {
        bReturn = true;
    }

    // Try to Connect
    dc = daveNewConnection(di,plcMPI,0,0);

    if(plc2MPI>=0)
        dc2 =daveNewConnection(di,plc2MPI,0,0);
    else
        dc2=NULL;
    printf("ConnectPLC\n");


    if (0==daveConnectPLC(dc)) {
        if(plc2MPI>=0) {
            if (0==daveConnectPLC(dc2)) ;
        } else {
            dc2=NULL;
        }


    } else {
            printf("Couldn't connect to PLC.\n Please try again. You may also try the option -2 for some adapters.\n");
}

    } else {
        printf("Couldn't open access point /S7online\n");
        //return -1;
        }
    return bReturn;
}

void S7Connection::diconnect()
{
    // Disconnect
    ;
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
