#include "timeoutread.h"
#include "select.h"
#include "error.h"
#include "readwrite.h"

static int secs;
static int flag;

void timeoutread_init(s) int s; { secs = s; flag = 0; }

int timeoutread(fd,buf,len) int fd; char *buf; int len;
{
 fd_set rfds;
 struct timeval tv;

 if (flag) { errno = error_timeout; return -1; }

 FD_ZERO(&rfds);
 FD_SET(fd,&rfds);
 tv.tv_sec = secs; tv.tv_usec = 0;
 if (select(fd + 1,&rfds,(fd_set *) 0,(fd_set *) 0,&tv) == -1) return -1;

 if (FD_ISSET(fd,&rfds)) return read(fd,buf,len);

 flag = 1; errno = error_timeout; return -1;
}
