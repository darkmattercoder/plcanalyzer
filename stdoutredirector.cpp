#include "stdoutredirector.h"

stdoutRedirector::stdoutRedirector()
{
    res=pipe(fds);
    assert(res==0);

    so=fileno(stdout);
    // close stdout handle and make the writable part of fds the new stdout.
    res=dup2(fds[1],so);
    assert(res!=-1);
}

stdoutRedirector::~stdoutRedirector()
{

}

void stdoutRedirector::run()
{

    Q_FOREVER{
    res=read(fds[0],buf,sizeof(buf)-1);
    assert(res>=0 && res<sizeof(buf));
    buf[res]=0;
    fprintf(stderr,"printf: %s\n",buf);
    }

}

fflushThread::fflushThread()
{

}

fflushThread::~fflushThread()
{

}

void fflushThread::run()
{

    Q_FOREVER{
        fflush(stdout);
    }

    }
