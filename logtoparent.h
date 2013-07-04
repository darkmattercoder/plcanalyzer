#ifndef LOGTOPARENT_H
#define LOGTOPARENT_H
#include "mainwindow.h"
#include <QMainWindow>
#include <QString>
#include "logtoparent.h"

class logToParent : public QMainWindow
{
    Q_OBJECT
public:
    explicit logToParent(MainWindow *parent = 0);
    QTextEdit *parentLog;
signals:
    
public slots:

    void updateParentLog(QString logString);
    
};

#endif // LOGTOPARENT_H
