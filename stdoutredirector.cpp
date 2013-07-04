#include "stdoutredirector.h"

stdoutRedirector::stdoutRedirector()
{
    res=pipe(fds);
    assert(res==0);

    so=fileno(stdout);
    // close stdout handle and make the writable part of fds the new stdout.
    res=dup2(fds[1],so);
    assert(res!=-1);
    terminate_ = false;
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
    qDebug() << buf;
    //fprintf(stderr,"printf: %s\n",buf);
    if (terminate_)
    {
        return;
    }
    }


}
void stdoutRedirector::terminate()
{
    terminate_ = true;
}

