#ifndef LOGTOPARENT_H
#define LOGTOPARENT_H
#include "mainwindow.h"
#include <QMainWindow>
#include <QString>

class logToParent : public QMainWindow
{
    Q_OBJECT
public:
    explicit logToParent(MainWindow *parent = 0);
    ~logToParent();
    QStatusBar *parentStatbar;
signals:
    void threadTerminator();
public slots:

    void updateParentLog(QString logString);
    
};

#endif // LOGTOPARENT_H
