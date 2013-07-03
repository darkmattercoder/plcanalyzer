#ifndef STDOUTREDIRECTOR_H
#define STDOUTREDIRECTOR_H

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
#include <QObject>

class stdoutRedirector : public QObject
{
public:

    stdoutRedirector();
    ~stdoutRedirector();
    int doRedirection();

public slots:

signals:



private:

    int fds[2];
    int res;
    char buf[256];
    int so;

};

#endif // STDOUTREDIRECTOR_H
