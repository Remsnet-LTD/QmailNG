#include "readwrite.h"
#include "wait.h"
#include "substdio.h"
#include "error.h"
#include "exit.h"
#include "fork.h"
#include "qqtalk.h"
#include "qqx.h"

static char *qqargs[2] = { "qmail-queue", 0 };
static char *binqqargs[2] = { "bin/qmail-queue", 0 } ;

int qqtalk_open(qqt,flagpath)
struct qqtalk *qqt;
int flagpath;
{
 int pim[2];
 int pie[2];

 if (pipe(pim) == -1) return -1;
 if (pipe(pie) == -1) { close(pim[0]); close(pim[1]); return -1; }
 
 switch(qqt->pid = vfork())
  {
   case -1:
     close(pim[0]); close(pim[1]);
     close(pie[0]); close(pie[1]);
     return -1;
   case 0:
     close(0); dup(pim[0]); close(pim[0]); close(pim[1]);
     close(1); dup(pie[0]); close(pie[0]); close(pie[1]);
     if (flagpath) execvp(*qqargs,qqargs);
     else execvp(*binqqargs,binqqargs);
     if (error_temp(errno)) _exit(QQX_EXECSOFT);
     _exit(QQX_EXECHARD);
  }

 qqt->fdm = pim[1]; close(pim[0]);
 qqt->fde = pie[1]; close(pie[0]);
 substdio_fdbuf(&qqt->ss,write,qqt->fdm,qqt->buf,sizeof(qqt->buf));
 qqt->flagerr = 0;
 return 0;
}

unsigned long qqtalk_qp(qqt) struct qqtalk *qqt;
{
 return qqt->pid;
}

void qqtalk_put(qqt,s,len) struct qqtalk *qqt; char *s; int len;
{
 if (qqt->flagerr) return;
 if (substdio_put(&qqt->ss,s,len) == -1) qqt->flagerr = 1;
}

void qqtalk_puts(qqt,s) struct qqtalk *qqt; char *s;
{
 if (qqt->flagerr) return;
 if (substdio_puts(&qqt->ss,s) == -1) qqt->flagerr = 1;
}

void qqtalk_from(qqt,s) struct qqtalk *qqt; char *s;
{
 if (substdio_flush(&qqt->ss) == -1) qqt->flagerr = 1;
 close(qqt->fdm);
 substdio_fdbuf(&qqt->ss,write,qqt->fde,qqt->buf,sizeof(qqt->buf));
 if (qqt->flagerr) return;
 if (substdio_put(&qqt->ss,"F",1) == -1) { qqt->flagerr = 1; return; }
 if (substdio_puts(&qqt->ss,s) == -1) { qqt->flagerr = 1; return; }
 if (substdio_put(&qqt->ss,"",1) == -1) { qqt->flagerr = 1; return; }
}

void qqtalk_to(qqt,s) struct qqtalk *qqt; char *s;
{
 if (qqt->flagerr) return;
 if (substdio_put(&qqt->ss,"T",1) == -1) { qqt->flagerr = 1; return; }
 if (substdio_puts(&qqt->ss,s) == -1) { qqt->flagerr = 1; return; }
 if (substdio_put(&qqt->ss,"",1) == -1) { qqt->flagerr = 1; return; }
}

void qqtalk_eput(qqt,s,len) struct qqtalk *qqt; char *s; int len;
{
 if (qqt->flagerr) return;
 if (substdio_put(&qqt->ss,s,len) == -1) { qqt->flagerr = 1; return; }
}

void qqtalk_fail(qqt) struct qqtalk *qqt;
{
 qqt->flagerr = 1;
}

int qqtalk_close(qqt)
struct qqtalk *qqt;
{
 int wstat;

 if (!qqt->flagerr)
   if (substdio_putflush(&qqt->ss,"",1) == -1)
     qqt->flagerr = 1;
 close(qqt->fde);

 if (wait_pid(&wstat,qqt->pid) != qqt->pid) return QQT_WAITPID;
 if (wait_crashed(wstat)) return QQT_CRASHED;
 switch(wait_exitcode(wstat))
  {
   case 0: if (qqt->flagerr) return QQT_BUG; return 0;
   case QQX_EXECHARD: return QQT_EXECHARD;
   case QQX_USAGE: return QQT_USAGE;
   case QQX_TOOLONG: return QQT_TOOLONG;
   case QQX_PIDUSED: case QQX_STAT: case QQX_LINK1:
   case QQX_LINK2: case QQX_TODO:
     return QQT_SYS;
   case QQX_READ: return QQT_READ;
   case QQX_WRITE: return QQT_WRITE;
   case QQX_NOMEM: return QQT_NOMEM;
   case QQX_TIMEOUT: return QQT_TIMEOUT;
   case QQX_EXECSOFT: return QQT_EXECSOFT;
   default: return QQT_BUG;
  }
}
