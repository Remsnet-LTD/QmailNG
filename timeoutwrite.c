#include "timeoutwrite.h"
#include "select.h"
#include "error.h"
#include "readwrite.h"

static int secs;
static int flag;

void timeoutwrite_init(s) int s; { secs = s; flag = 0; }

int timeoutwrite(fd,buf,len) int fd; char *buf; int len;
{
 fd_set wfds;
 struct timeval tv;

 if (flag) { errno = error_timeout; return -1; }

 FD_ZERO(&wfds);
 FD_SET(fd,&wfds);
 tv.tv_sec = secs; tv.tv_usec = 0;
 if (select(fd + 1,(fd_set *) 0,&wfds,(fd_set *) 0,&tv) == -1) return -1;

 if (FD_ISSET(fd,&wfds)) return write(fd,buf,len);

 flag = 1; errno = error_timeout; return -1;
}
