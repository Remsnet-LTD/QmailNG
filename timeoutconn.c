#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "timeoutconn.h"
#include "ndelay.h"
#include "select.h"
#include "error.h"
#include "readwrite.h"
#include "ip.h"

int timeoutconn(s,ip,port,timeout)
int s;
struct ip_address *ip;
unsigned int port;
int timeout;
{
 char ch;
 struct sockaddr_in sin;
 char *x;
 fd_set wfds;
 struct timeval tv;

 for (x = (char *) &sin;x < (char *) &sin + sizeof(sin);++x) *x = 0;
 x = (char *) &sin.sin_addr;
 x[0] = ip->d[0]; x[1] = ip->d[1]; x[2] = ip->d[2]; x[3] = ip->d[3];
 x = (char *) &sin.sin_port;
 x[1] = port; port >>= 8; x[0] = port;
 sin.sin_family = AF_INET;

 if (ndelay(s) == -1) return -1;

 /* XXX: could bind s */

 if (connect(s,(struct sockaddr *) &sin,sizeof(sin)) == 0) return 0;
 if ((errno != error_inprogress) && (errno != error_wouldblock)) return -1;

 FD_ZERO(&wfds);
 FD_SET(s,&wfds);
 tv.tv_sec = timeout; tv.tv_usec = 0;

 if (select(s + 1,(fd_set *) 0,&wfds,(fd_set *) 0,&tv) == -1) return -1;
 if (FD_ISSET(s,&wfds))
  {
   int dummy;
   dummy = sizeof(sin);
   if (getpeername(s,(struct sockaddr *) &sin,&dummy) == -1)
    {
     read(s,&ch,1);
     return -1;
    }
   return 0;
  }

 errno = error_timeout; /* note that connect attempt is continuing */
 return -1;
}
