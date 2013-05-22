#include "s7connection.h"


// Konstruktor
S7Connection::S7Connection()
{
    // DLL IMPORT
    QFile file;
    file.setFileName("s7onlinx.dll");
    if(file.exists()==true){
        QLibrary myLib("S7onlinx");
        if(myLib.load()==true){
            if(myLib.isLoaded()==true){
                //openS7online = (_openS7online)(myLib.resolve("SCP_open"));
                SCP_open = (_openFunc)(myLib.resolve("SCP_open"));
                SCP_close = (_closeFunc)(myLib.resolve("SCP_close"));
                SCP_send = (_sendFunc)(myLib.resolve("SCP_send"));
                SCP_receive = (_receiveFunc)(myLib.resolve("SCP_receive"));
                SCP_get_errno = (_get_errnoFunc)(myLib.resolve("SCP_get_errno"));
                SetSinecHWnd = (_setHWnd) (myLib.resolve("SetSinecHWnd"));

                if (true){
                    // inopenS7online("/S/online");
                    //msgBox.setText("Die Funktion GetSerial wurde erfolgreich geladen!");
                    //msgBox.exec();
                }else{
                    msgBox.setText("Die Funktion GetSerial konnte nicht geladen werden!\n"+myLib.errorString());
                    msgBox.exec();
                }
            }else{
                msgBox.setText("Die DLL libCol wurde nicht geladen!\n"+myLib.errorString());
                msgBox.exec();
            }
        }else{
            msgBox.setText("Die DLL libCol konnte nicht geladen werden!\n"+myLib.errorString());
            msgBox.exec();
        }
    }else{
        msgBox.setText("Die DLL libCol.dll existiert nicht an dem angegebenen Ort!");
        msgBox.exec();
    }


    // Initialisize Variables
    localMPI = 0;
    useProto = daveProtoS7online;
    speed = daveSpeed187k;
    plcMPI = 2;
    plc2MPI = -1;
}

// Destructor
S7Connection::~S7Connection()
{
    printf("Deconstructor\n");
    //daveDisconnectAdapter(di);
    //closeS7online(fds.rfd);
    //daveFree(di);
}

// Opens the Connection
bool S7Connection::startConnection(HWND WndHandle)
{
    bool bReturn = false;
    fds.rfd = NULL;
    fds.rfd = openS7online("/S7ONLINE", WndHandle);
    printf("OpenS7online Handle: %i\n", (int) fds.rfd);
    fds.wfd=fds.rfd;
    if (((int)fds.rfd)>=0) { 	// had Problems here: HANDLE is unsigned, 0 is a valid HANDLE for s7onlinx

        // daveSetDebug(1);
        printf("Debug Status %i",daveGetDebug());


        di = daveNewInterface(fds, "IF1", localMPI, useProto, speed);

        int initSuccess = 0;
        char buf1 [davePartnerListSize];

        daveSetTimeout(di,5000000);
        printf("DI lautet: %i", int(di));
        msgBox.setText("DaveTimeout meldet\n");
        msgBox.exec();
        int a;

        for (int i=0; i<3; i++) {
            if (0==daveInitAdapter(di)) {
            initSuccess=1;
            a= daveListReachablePartners(di,buf1);
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

        msgBox.setText("Ausgabe der Partner erfolgreich\n");
        msgBox.exec();

        if (!initSuccess) {
            printf("Couldn't connect to Adapter!.\n Please try again. You may also try the option -2 for some adapters.\n");
        } else {
            bReturn = true;
        }

        // Try to Connect
        dc = daveNewConnection(di,plcMPI,0,0);

        msgBox.setText("New Connection erfolgreich\n");
        msgBox.exec();

        if(plc2MPI>=0)
            dc2 =daveNewConnection(di,plc2MPI,0,0);
        else
            dc2=NULL;


        if (0==daveConnectPLC(dc))
        {
            msgBox.setText("Try to connect erfolgreich\n");
            msgBox.exec();

            if(plc2MPI>=0)
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
            printf("Couldn't connect to PLC.\n Please try again. You may also try the option -2 for some adapters.\n");
            msgBox.setText("Couldn't connect to PLC.\n Please try again. You may also try the option -2 for some adapters.\n");
            msgBox.exec();
        }

    } else {
        printf("Couldn't connect to PLC.\n Please try again. You may also try the option -2 for some adapters.\n");
        //return -1;
    }
    return bReturn;
}


void S7Connection::diconnect()
{
    // Disconnect
    printf("Disconnect");
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

HANDLE S7Connection::openS7online(const char *accessPoint, HWND handle)
{
    int h, en;
    en=SCP_get_errno();
    h=SCP_open(reinterpret_cast<unsigned const char *>(accessPoint));
    en=SCP_get_errno();
    SetSinecHWnd(h, handle);
    return (HANDLE) h;
}

HANDLE S7Connection::closeS7online(int h)
{
    SCP_close(h);
    return NULL;
}
