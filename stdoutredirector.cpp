#include "stdoutredirector.h"

stdoutRedirector::stdoutRedirector()
{

}

stdoutRedirector::~stdoutRedirector()
{

}

int stdoutRedirector::doRedirection()
{

    res=pipe(fds);
    assert(res==0);

    so=fileno(stdout);
    // close stdout handle and make the writable part of fds the new stdout.
    res=dup2(fds[1],so);
    assert(res!=-1);

 //   printf("Hi there\n");
  //  fflush(stdout);
    // reading should happen in a different thread

    res=read(fds[0],buf,sizeof(buf)-1);
    assert(res>=0 && res<sizeof(buf));
    buf[res]=0;
    fprintf(stderr,"buf=>%s\n",buf);
    return 0;


}
