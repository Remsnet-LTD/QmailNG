#include <sys/types.h>
#include <fcntl.h>
#include "ndelay.h"

int ndelay(fd)
int fd;
{
 int flags;
 flags = fcntl(fd,F_GETFL,0);
 return fcntl(fd,F_SETFL,flags | O_NDELAY);
}
