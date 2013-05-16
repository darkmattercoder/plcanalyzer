#include <QtGui/QApplication>
#include "mainwindow.h"
#include <stdlib.h>
#include <stdio.h>
//#include <string.h>
#include "s7connection.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // w.winId()


    return a.exec();
}



