#include "subfd.h"
#include "substdio.h"
#include "ip.h"
#include "ipme.h"
#include "exit.h"

char temp[IPFMT];

void main()
{
 int j;
 switch(ipme_init())
  {
   case 0: substdio_putsflush(subfderr,"out of memory\n"); _exit(1);
   case -1: substdio_putsflush(subfderr,"hard error\n"); _exit(1);
  }
 for (j = 0;j < ipme.len;++j)
  {
   substdio_put(subfdout,temp,ip_fmt(temp,&ipme.ix[j].ip));
   substdio_putsflush(subfdout,"\n");
  }
 _exit(0);
}
