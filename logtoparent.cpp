#include "logtoparent.h"
using namespace std;



logToParent::logToParent(MainWindow *parent) :
    QMainWindow(parent)
{
    myRedirector = new stdoutRedirector;
    connect(this, SIGNAL(threadTerminator()),myRedirector,SLOT(terminate()));
    cout << "Debug: Qthread for logging run: " << myRedirector->isRunning() << endl;
    myRedirector->start();
    cout << "Debug: Logger constructed" << endl;
    cout << "Debug: Qthread for logging run: " << myRedirector->isRunning() << endl;
    parentStatbar = parent->ui->statusBar;
    connect(myRedirector, SIGNAL(writeOut(QString)),this,SLOT(updateParentLog(QString)));
}

logToParent::~logToParent()
{

emit threadTerminator();
}

void logToParent::updateParentLog(QString logString)
{
    parentStatbar->showMessage(logString,2500);

}
