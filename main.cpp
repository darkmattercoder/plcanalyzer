//uncommented for strange linux compile errors
//#include <QtGui/QApplication>
#include "mainwindow.h"




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    setvbuf(stdout, NULL, _IONBF, 0);
    w.show();



    return a.exec();
}



