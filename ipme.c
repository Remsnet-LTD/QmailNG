#include <sys/types.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#ifndef SIOCGIFCONF /* whatever works */
#include <sys/sockio.h>
#endif
#include "auto-hassalen.h"
#include "byte.h"
#include "ip.h"
#include "ipalloc.h"
#include "ipme.h"

static int ipmeok = 0;
ipalloc ipme = {0};

int ipme_is(ip)
struct ip_address *ip;
{
 int i;
 if (ipme_init() != 1) return -1;
 for (i = 0;i < ipme.len;++i)
   if (ipme.ix[i].ip.d[0] == ip->d[0]) if (ipme.ix[i].ip.d[1] == ip->d[1])
   if (ipme.ix[i].ip.d[2] == ip->d[2]) if (ipme.ix[i].ip.d[3] == ip->d[3])
     return 1;
 return 0;
}

int ipme_init()
{
 char buf[1024];
 struct ifconf ifc;
 char *x;
 struct ifreq *ifr;
 struct sockaddr_in *sin;
 int len;
 int s;

 if (ipmeok) return 1;
 if (!ipalloc_readyplus(&ipme,0)) return 0;
 ipme.len = 0;

 if ((s = socket(AF_INET,SOCK_STREAM,0)) == -1) return -1;
 ifc.ifc_buf = buf;
 ifc.ifc_len = sizeof(buf);
 if (ioctl(s,SIOCGIFCONF,&ifc) == -1) { close(s); return -1; }
 x = buf;
 while (x < buf + ifc.ifc_len)
  {
   ifr = (struct ifreq *) x;
#ifdef HASSALEN
   len = sizeof(ifr->ifr_name) + ifr->ifr_addr.sa_len;
   if (len < sizeof(*ifr))
#endif
     len = sizeof(*ifr);
   if (ioctl(s,SIOCGIFFLAGS,x) == 0)
     if (ifr->ifr_flags & IFF_UP)
       if (ioctl(s,SIOCGIFADDR,x) == 0)
	 if (ifr->ifr_addr.sa_family == AF_INET)
	  {
	   struct ip_mx ix;
	   sin = (struct sockaddr_in *) &ifr->ifr_addr;
	   byte_copy(&ix.ip,4,&sin->sin_addr);
	   ix.pref = 0;
	   if (!ipalloc_append(&ipme,&ix)) return 0;
	  }
   x += len;
  }
 close(s);
 ipmeok = 1;
 return 1;
}
