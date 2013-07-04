#include "logtoparent.h"

logToParent::logToParent(MainWindow *parent) :
    QMainWindow(parent)
{
    parentLog = parent->ui->textEdit;

}


void logToParent::updateParentLog(QString logString)
{
    parentLog->append(logString);
}
