#include <sys/types.h>
#include <sys/stat.h>
#include "readwrite.h"
#include "signal.h"
#include "exit.h"
#include "open.h"
#include "seek.h"
#include "fmt.h"
#include "alloc.h"
#include "substdio.h"
#include "datetime.h"
#include "now.h"
#include "triggerpull.h"
#include "conf-home.h"
#include "auto-uids.h"
#include "qqx.h"
#include "date822fmt.h"
#include "fmtqfn.h"

#define DEATH 86400 /* 24 hours; _must_ be below q-s's OSSIFIED (36 hours) */
#define ADDR 1003

char inbuf[2048];
struct substdio ssin;
char outbuf[256];
struct substdio ssout;

datetime_sec starttime;
struct datetime dt;
unsigned long mypid;
unsigned long uid;
char *pidfn;
struct stat pidst;
unsigned long messnum;
char *messfn;
char *todofn;
char *intdfn;
int messfd;
int intdfd;
int flagmademess = 0;
int flagmadeintd = 0;

void cleanup()
{
 if (flagmadeintd)
  {
   seek_trunc(intdfd,0);
   if (unlink(intdfn) == -1) return;
  }
 if (flagmademess)
  {
   seek_trunc(messfd,0);
   if (unlink(messfn) == -1) return;
  }
}

void die(e) int e; { _exit(e); }
void die_write() { cleanup(); die(QQX_WRITE); }
void die_read() { cleanup(); die(QQX_READ); }
void sigalrm() { /* thou shalt not clean up here */ die(QQX_TIMEOUT); }
void sigbug() { die(QQX_BUG); }

unsigned int receivedlen;
char *received;
/* "Received: (qmail-queue invoked by alias); 26 Sep 1995 04:46:54 -0000\n" */

static unsigned int receivedfmt(s)
char *s;
{
 unsigned int i;
 unsigned int len;
 len = 0;
 i = fmt_str(s,"Received: (qmail "); len += i; if (s) s += i;
 i = fmt_ulong(s,mypid); len += i; if (s) s += i;
 i = fmt_str(s," invoked "); len += i; if (s) s += i;
 if (uid == UID_ALIAS)
  { i = fmt_str(s,"by alias"); len += i; if (s) s += i; }
 else if (uid == UID_DAEMON)
  { i = fmt_str(s,"from network"); len += i; if (s) s += i; }
 else if (uid == UID_SEND)
  { i = fmt_str(s,"for bounce"); len += i; if (s) s += i; }
 else
  {
   i = fmt_str(s,"by uid "); len += i; if (s) s += i;
   i = fmt_ulong(s,uid); len += i; if (s) s += i;
  }
 i = fmt_str(s,"); "); len += i; if (s) s += i;
 i = date822fmt(s,&dt); len += i; if (s) s += i;
 return len;
}

void received_setup()
{
 receivedlen = receivedfmt((char *) 0);
 received = alloc(receivedlen + 1);
 if (!received) die(QQX_NOMEM);
 receivedfmt(received);
}

unsigned int pidfmt(s,seq)
char *s;
unsigned long seq;
{
 unsigned int i;
 unsigned int len;

 len = 0;
 i = fmt_str(s,"pid/"); len += i; if (s) s += i;
 i = fmt_ulong(s,mypid); len += i; if (s) s += i;
 i = fmt_str(s,"."); len += i; if (s) s += i;
 i = fmt_ulong(s,starttime); len += i; if (s) s += i;
 i = fmt_str(s,"."); len += i; if (s) s += i;
 i = fmt_ulong(s,seq); len += i; if (s) s += i;
 ++len; if (s) *s++ = 0;

 return len;
}

char *fnnum(dirslash,flagsplit)
char *dirslash;
int flagsplit;
{
 char *s;

 s = alloc(fmtqfn((char *) 0,dirslash,messnum,flagsplit));
 if (!s) die(QQX_NOMEM);
 fmtqfn(s,dirslash,messnum,flagsplit);
 return s;
}

void pidopen()
{
 unsigned int len;
 unsigned long seq;

 seq = 1;
 len = pidfmt((char *) 0,seq);
 pidfn = alloc(len);
 if (!pidfn) die(QQX_NOMEM);

 for (seq = 1;seq < 10;++seq)
  {
   if (pidfmt((char *) 0,seq) > len) die(QQX_BUG); /* paranoia */
   pidfmt(pidfn,seq);
   messfd = open_excl(pidfn);
   if (messfd != -1) return;
  }

 die(QQX_PIDUSED);
}

char tmp[FMT_ULONG];

void main()
{
 unsigned int len;
 char ch;

 signal_blocknone();
 umask(033);
 if (chdir(CONF_HOME) == -1) die(QQX_CHDIR);
 if (chdir("queue") == -1) die(QQX_CHDIR);

 mypid = getpid();
 uid = getuid();
 starttime = now();
 datetime_tai(&dt,starttime);

 received_setup();

 signal_init();
 signal_ignoremisc();
 signal_catchalarm(sigalrm);
 signal_catchbug(sigbug);

 alarm(DEATH);

 pidopen();
 if (fstat(messfd,&pidst) == -1) die(QQX_STAT);

 messnum = pidst.st_ino;
 messfn = fnnum("mess/",1);
 todofn = fnnum("todo/",0);
 intdfn = fnnum("intd/",0);

 if (link(pidfn,messfn) == -1) die(QQX_LINK1);
 if (unlink(pidfn) == -1) die(QQX_LINK1);
 flagmademess = 1;

 substdio_fdbuf(&ssout,write,messfd,outbuf,sizeof(outbuf));
 substdio_fdbuf(&ssin,read,0,inbuf,sizeof(inbuf));

 if (substdio_bput(&ssout,received,receivedlen) == -1) die_write();

 switch(substdio_copy(&ssout,&ssin))
  {
   case -2: die_read();
   case -3: die_write();
  }

 if (substdio_flush(&ssout) == -1) die_write();
 if (fsync(messfd) == -1) die_write();

 intdfd = open_excl(intdfn);
 if (intdfd == -1) die(QQX_TODO);
 flagmadeintd = 1;

 substdio_fdbuf(&ssout,write,intdfd,outbuf,sizeof(outbuf));
 substdio_fdbuf(&ssin,read,1,inbuf,sizeof(inbuf));

 if (substdio_bput(&ssout,"u",1) == -1) die_write();
 if (substdio_bput(&ssout,tmp,fmt_ulong(tmp,uid)) == -1) die_write();
 if (substdio_bput(&ssout,"",1) == -1) die_write();

 if (substdio_bput(&ssout,"p",1) == -1) die_write();
 if (substdio_bput(&ssout,tmp,fmt_ulong(tmp,mypid)) == -1) die_write();
 if (substdio_bput(&ssout,"",1) == -1) die_write();

 if (substdio_get(&ssin,&ch,1) < 1) die_read();
 if (ch != 'F') die(QQX_USAGE);
 if (substdio_bput(&ssout,&ch,1) == -1) die_write();
 for (len = 0;len < ADDR;++len)
  {
   if (substdio_get(&ssin,&ch,1) < 1) die_read();
   if (substdio_put(&ssout,&ch,1) == -1) die_write();
   if (!ch) break;
  }
 if (len >= ADDR) die(QQX_TOOLONG);

 for (;;)
  {
   if (substdio_get(&ssin,&ch,1) < 1) die_read();
   if (!ch) break;
   if (ch != 'T') die(QQX_USAGE);
   if (substdio_bput(&ssout,&ch,1) == -1) die_write();
   for (len = 0;len < ADDR;++len)
    {
     if (substdio_get(&ssin,&ch,1) < 1) die_read();
     if (substdio_bput(&ssout,&ch,1) == -1) die_write();
     if (!ch) break;
    }
   if (len >= ADDR) die(QQX_TOOLONG);
  }

 if (substdio_flush(&ssout) == -1) die_write();
 if (fsync(intdfd) == -1) die_write();

 if (link(intdfn,todofn) == -1) die(QQX_LINK2);

 triggerpull();
 die(0);
}
