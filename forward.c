#include "signal.h"
#include "readwrite.h"
#include "exit.h"
#include "env.h"
#include "qqtalk.h"
#include "stralloc.h"
#include "subfd.h"
#include "substdio.h"

void die_success() { _exit(0); }
void die_perm(s) char *s; { substdio_putsflush(subfderr,s); _exit(1); }
void die_temp(s) char *s; { substdio_putsflush(subfderr,s); _exit(111); }
void die_nomem() { die_temp("forward: fatal: out of memory\n"); }

struct qqtalk qqt;

int mywrite(fd,buf,len) int fd; char *buf; int len;
{
 qqtalk_put(&qqt,buf,len);
 return len;
}

substdio ssin;
substdio ssout;
char inbuf[SUBSTDIO_INSIZE];
char outbuf[16];

void main(argc,argv)
int argc;
char **argv;
{
 char *sender;
 char *dtline;

 signal_init();

 if (!argv[1]) die_perm("forward: usage: forward newaddress ...\n");
 sender = env_get("NEWSENDER");
 if (!sender) die_perm("forward: fatal: NEWSENDER not set\n");
 dtline = env_get("DTLINE");
 if (!dtline) die_perm("forward: fatal: DTLINE not set\n");

 if (qqtalk_open(&qqt,1) == -1) die_temp("forward: fatal: unable to fork\n");
 qqtalk_puts(&qqt,dtline);
 substdio_fdbuf(&ssin,read,0,inbuf,sizeof(inbuf));
 substdio_fdbuf(&ssout,mywrite,-1,outbuf,sizeof(outbuf));
 if (substdio_copy(&ssout,&ssin) != 0)
   die_temp("forward: fatal: error while reading message\n");
 substdio_flush(&ssout);

 qqtalk_from(&qqt,sender);
 while (*++argv) qqtalk_to(&qqt,*argv);
 switch(qqtalk_close(&qqt))
  {
   case 0: die_success();
   case QQT_TOOLONG: die_perm("forward: fatal: permanent qmail-queue error\n");
   default: die_temp("forward: fatal: temporary qmail-queue error\n");
  }
}
