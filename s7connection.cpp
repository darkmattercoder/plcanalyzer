#include "s7connection.h"
#include "log2.h"

//#define printf(x) ui->textEdit->append(QString().sprintf(x));

// Konstruktor
S7Connection::S7Connection()
{
    // Initialisize Variables
    MyConSet.localMPI = 0;
    MyConSet.useProto = daveProtoS7online;
    MyConSet.speed = daveSpeed187k;
    MyConSet.plcMPI = 2;
    MyConSet.plc2MPI = -1;
    MyConSet.IP_Adr = "192.168.0.1";
    initSuccess = 0;
}

// Destructor
S7Connection::~S7Connection()
{
    disconnect();

}

// Opens the Connection
bool S7Connection::startConnection(HWND WndHandle)
{
    if(!isConnected())
    {
        fds.rfd = NULL;
        fds.rfd = openS7online("/S7ONLINE", WndHandle);
        LOG2("OpenS7online Handle:",(int) fds.rfd);
        FLUSH;

        fds.wfd=fds.rfd;
        if (((int)fds.rfd)>=0) { 	// had Problems here: HANDLE is unsigned, 0 is a valid HANDLE for s7onlinx

            daveSetDebug(daveDebugListReachables);

            di = daveNewInterface(fds, "IF1", MyConSet.localMPI, MyConSet.useProto, MyConSet.speed);

            char buf1 [davePartnerListSize];

            daveSetTimeout(di,5000000);

            msgBox.setText("Dave Timeout ausgefuehrt\n");
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

            if (!initSuccess)
            {
                printf("Couldn't connect to Adapter!.\n Please try again. You may also try the option -2 for some adapters.\n");
            }

            // Try to Connect
            dc = daveNewConnection(di,MyConSet.plcMPI,0,0);

            msgBox.setText("New Connection erfolgreich\n");
            msgBox.exec();

            if(MyConSet.plc2MPI>=0)
                dc2 =daveNewConnection(di,MyConSet.plc2MPI,0,0);
            else
                dc2=NULL;


            if (0==daveConnectPLC(dc))
            {
                msgBox.setText("Try to connect erfolgreich\n");
                msgBox.exec();

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
                printf("Couldn't connect to PLC.\n Please try again. You may also try the option -2 for some adapters.\n");
                daveDisconnectAdapter(di);
                closeS7online(int(fds.rfd));
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


void S7Connection::disconnect()
{
    // Disconnect from PLC
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
        closeS7online(int(fds.rfd));
        daveFree(di);

        LOG1("Deconstructor\n");
        FLUSH;
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
