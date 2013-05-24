#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QLibrary>
#include "nodave.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    typedef int (DECL2 * _setHWnd) (int, HWND);
//    _openFunc SCP_open;
//    _closeFunc SCP_close;
//    _sendFunc SCP_send;
//    _receiveFunc SCP_receive;
//    _setHWnd SetSinecHWnd;
//    _get_errnoFunc SCP_get_errno;

//     QLibrary myLib("S7onlinx");
//    SCP_open = (_openFunc)(myLib.resolve("SCP_open"));
//    printf("Adresse SCP_open: %i\n", int(SCP_open));
//    SCP_close = (_closeFunc)(myLib.resolve("SCP_close"));
//    printf("Adresse SCP_close: %i\n", int(SCP_close));
//    SCP_send = (_sendFunc)(myLib.resolve("SCP_send"));
//    printf("Adresse SCP_send: %i\n", int(SCP_send));
//    SCP_receive = (_receiveFunc)(myLib.resolve("SCP_receive"));
//    SCP_get_errno = (_get_errnoFunc)(myLib.resolve("SCP_get_errno"));
//    SetSinecHWnd = (_setHWnd) (myLib.resolve("SetSinecHWnd"));


    return a.exec();
}



