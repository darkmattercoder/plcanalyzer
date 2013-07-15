#ifndef STDOUTREDIRECTOR_H
#define STDOUTREDIRECTOR_H
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QMainWindow>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
/* gcc defined unix */
#ifdef LINUX
#include <unistd.h>
#endif
#ifdef BCCWIN
#include <io.h>
#define pipe(X) _pipe(X,4096,O_BINARY)
#define fileno _fileno
#define dup2 _dup2
#define read _read

#endif
#include <assert.h>


class stdoutRedirector :  public QThread
{
      Q_OBJECT
public:

    stdoutRedirector();
    ~stdoutRedirector();
    void run();
signals:
    void writeOut(QString);
public slots:
    void terminate();

private:

    int fds[2];
    int res;
    char buf[256];
    int so;
    bool terminate_;

};


#endif // STDOUTREDIRECTOR_H
