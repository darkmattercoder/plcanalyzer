#define LINUX
#include <QtGui/QApplication>
#include <iostream>
#include "mainwindow.h"
using namespace std;
void printUnnuetzesZeug();
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
printUnnuetzesZeug();
    return a.exec();
}

void printUnnuetzesZeug()
{cout << "Hello LuKe" << endl;}
