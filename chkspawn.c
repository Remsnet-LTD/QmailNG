#include "substdio.h"
#include "subfd.h"
#include "fmt.h"
#include "select.h"
#include "exit.h"
#include "conf-unusual.h"

char num[FMT_ULONG];
fd_set fds;

void main()
{
 unsigned long hiddenlimit;
 unsigned long maxnumd;

 hiddenlimit = sizeof(fds) * 8;
 maxnumd = (hiddenlimit - 5) / 2;

 if (SPAWN_NUMD < 1)
  {
   substdio_puts(subfdout,"Oops. You have set SPAWN_NUMD in conf-unusual.h lower than 1.\n");
   substdio_flush(subfdout);
   _exit(1);
  }

 if (SPAWN_NUMD > 255)
  {
   substdio_puts(subfdout,"Oops. You have set SPAWN_NUMD in conf-unusual.h higher than 255.\n");
   substdio_flush(subfdout);
   _exit(1);
  }

 if (SPAWN_NUMD > maxnumd)
  {
   substdio_puts(subfdout,"Oops. Your system's FD_SET() has a hidden limit of ");
   substdio_put(subfdout,num,fmt_ulong(num,hiddenlimit));
   substdio_puts(subfdout," descriptors.\n\
This means that the qmail daemons could crash if you set the run-time\n\
concurrency higher than ");
   substdio_put(subfdout,num,fmt_ulong(num,maxnumd));
   substdio_puts(subfdout,". So I'm going to insist that SPAWN_NUMD, the\n\
concurrency limit in conf-unusual.h, be at most ");
   substdio_put(subfdout,num,fmt_ulong(num,maxnumd));
   substdio_puts(subfdout,". Right now it's ");
   substdio_put(subfdout,num,fmt_ulong(num,(unsigned long) SPAWN_NUMD));
   substdio_puts(subfdout,".\n");
   substdio_flush(subfdout);
   _exit(1);
  }
 _exit(0);
}
