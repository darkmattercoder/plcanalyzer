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

signals:
    void threadTerminator();
public slots:

    void updateParentLog(QString logString);
private:
    QStatusBar *parentStatbar;
    stdoutRedirector * myRedirector;
};

#endif // LOGTOPARENT_H
