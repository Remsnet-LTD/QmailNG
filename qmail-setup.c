#include <sys/types.h>
#include "subfd.h"
#include "readwrite.h"
#include "substdio.h"
#include "stralloc.h"
#include "open.h"
#include "fifo.h"
#include "fmt.h"
#include "exit.h"
#include "error.h"
#include "conf-home.h"
#include "conf-unusual.h"
#include "auto-uids.h"

void die(s,fn,t,err) char *s; char *fn; char *t; char *err;
{
 substdio_puts(subfderr,"qmail-setup: ");
 substdio_puts(subfderr,s);
 substdio_puts(subfderr,fn);
 substdio_puts(subfderr,t);
 substdio_puts(subfderr,err);
 substdio_puts(subfderr,"\n");
 substdio_flush(subfderr);
 _exit(1);
}
void die_nomem() { die("fatal: out of memory","","",""); }
void die_write(s) char *s; { die("fatal: unable to write ",s,": ",error_str(errno)); }
void die_read(s) char *s; { die("fatal: unable to read ", s,": ",error_str(errno)); }
void die_chmod(s) char *s; { die("fatal: unable to chmod ", s,": ",error_str(errno)); }
void die_chown(s) char *s; { die("fatal: unable to chown ", s,": ",error_str(errno)); }
void die_mkdir(s) char *s; { die("fatal: unable to mkdir ", s,": ",error_str(errno)); }
void die_mkfifo(s) char *s; { die("fatal: unable to mkfifo ", s,": ",error_str(errno)); }

stralloc qhfn = {0};

void makeqhfn(fn)
char *fn;
{
 if (!stralloc_copys(&qhfn,CONF_HOME)) die_nomem();
 if (!stralloc_cats(&qhfn,"/")) die_nomem();
 if (!stralloc_cats(&qhfn,fn)) die_nomem();
 if (!stralloc_0(&qhfn)) die_nomem();
}

void protect(fn,uid,mode)
char *fn;
int uid;
int mode;
{
 if (chown(fn,uid,GID_QMAIL) == -1) die_chown(fn);
 if (chmod(fn,mode) == -1) die_chmod(fn);
}

char inbuf[SUBSTDIO_INSIZE];
char outbuf[SUBSTDIO_OUTSIZE];

void copy(fnfrom,fn,uid,mode)
char *fnfrom;
char *fn;
int uid;
int mode;
{
 int fd;
 int fdfrom;
 substdio ssin;
 substdio ssout;

 makeqhfn(fn);
 fdfrom = open_read(fnfrom);
 if (fdfrom == -1) die_read(fnfrom);
 fd = open_trunc(qhfn.s);
 if (fd == -1) die_write(fn);

 substdio_fdbuf(&ssout,write,fd,outbuf,sizeof(outbuf));
 substdio_fdbuf(&ssin,read,fdfrom,inbuf,sizeof(inbuf));

 switch(substdio_copy(&ssout,&ssin))
  {
   case -2: die_read(fnfrom);
   case -3: die_write(fn);
  }

 if (substdio_flush(&ssout) == -1) die_write(fn);

 close(fdfrom);
 close(fd);
 protect(qhfn.s,uid,mode);
}

void zero1024(fn,uid,mode)
char *fn;
int uid;
int mode;
{
 int fd;
 substdio ssout;
 int i;

 makeqhfn(fn);
 fd = open_trunc(qhfn.s);
 if (fd == -1) die_write(fn);
 substdio_fdbuf(&ssout,write,fd,outbuf,sizeof(outbuf));
 for (i = 0;i < 1024;++i) substdio_put(&ssout,"",1);
 if (substdio_flush(&ssout) == -1) die_write(fn);
 close(fd);
 protect(qhfn.s,uid,mode);
}

void creatdir(fn,uid,mode)
char *fn;
int uid;
int mode;
{
 makeqhfn(fn);
 if (mkdir(qhfn.s,mode) == -1)
   if (errno != error_exist)
     die_mkdir(qhfn.s);
 protect(qhfn.s,uid,mode);
}

void creatdirsplit(fn,uid,mode)
char *fn;
int uid;
int mode;
{
 int i;
 char foo[40]; /*XXX*/
 creatdir(fn,uid,mode);
 for (i = 0;i < QFN_SPLIT;++i)
  {
   foo[fmt_ulong(foo,(unsigned long) i)] = 0;
   if (!stralloc_copys(&qhfn,CONF_HOME)) die_nomem();
   if (!stralloc_cats(&qhfn,"/")) die_nomem();
   if (!stralloc_cats(&qhfn,fn)) die_nomem();
   if (!stralloc_cats(&qhfn,"/")) die_nomem();
   if (!stralloc_cats(&qhfn,foo)) die_nomem();
   if (!stralloc_0(&qhfn)) die_nomem();
   if (mkdir(qhfn.s,mode) == -1)
     if (errno != error_exist)
       die_mkdir(qhfn.s);
   protect(qhfn.s,uid,mode);
  }
}

void creatfifo(fn,uid,mode)
char *fn;
int uid;
int mode;
{
 makeqhfn(fn);
 if (fifo_make(qhfn.s,mode) == -1)
   if (errno != error_exist)
     die_mkfifo(qhfn.s);
 protect(qhfn.s,uid,mode);
}

void main()
{
 umask(077);

 creatdir("control",UID_OWNER,0755);
 creatdir("users",UID_OWNER,0755);
 creatdir("queue",UID_QUEUE,0750);
 creatdir("bin",UID_OWNER,0755);
 creatdir("alias",UID_ALIAS,0755);
 creatdir("man",UID_OWNER,0755);
 creatdir("man/cat1",UID_OWNER,0755);
 creatdir("man/cat5",UID_OWNER,0755);
 creatdir("man/cat7",UID_OWNER,0755);
 creatdir("man/cat8",UID_OWNER,0755);

 creatdir("queue/pid",UID_QUEUE,0700);
 creatdir("queue/intd",UID_QUEUE,0700);
 creatdirsplit("queue/mess",UID_QUEUE,0750);
 creatdir("queue/todo",UID_QUEUE,0750);
 creatdirsplit("queue/info",UID_SEND,0700);
 creatdirsplit("queue/local",UID_SEND,0700);
 creatdirsplit("queue/remote",UID_SEND,0700);
 creatdir("queue/bounce",UID_SEND,0700);
 creatdir("queue/lock",UID_QUEUE,0750);

 creatfifo("queue/lock/trigger",UID_SEND,0622);
 copy("/dev/null","queue/lock/sendmutex",UID_SEND,0600);
 zero1024("queue/lock/tcpto",UID_REMOTE,0644);

 copy("qmail-alias","bin/qmail-alias",UID_OWNER,0755);
 copy("qmail-clean","bin/qmail-clean",UID_OWNER,0755);
 copy("qmail-home","bin/qmail-home",UID_OWNER,0755);
 copy("qmail-inject","bin/qmail-inject",UID_OWNER,0755);
 copy("qmail-lspawn","bin/qmail-lspawn",UID_OWNER,0755);
 copy("qmail-newuser","bin/qmail-newuser",UID_OWNER,0755);
 copy("qmail-pw2user","bin/qmail-pw2user",UID_OWNER,0755);
 copy("qmail-getpw","bin/qmail-getpw",UID_OWNER,0755);
 copy("qmail-qread","bin/qmail-qread",UID_OWNER,0755);
 copy("qmail-tcpto","bin/qmail-tcpto",UID_OWNER,0755);
 copy("qmail-qstat","bin/qmail-qstat",UID_OWNER,0755);
 copy("qmail-queue","bin/qmail-queue",UID_QUEUE,04755);
 copy("qmail-remote","bin/qmail-remote",UID_OWNER,0755);
 copy("qmail-rspawn","bin/qmail-rspawn",UID_OWNER,0755);
 copy("qmail-send","bin/qmail-send",UID_OWNER,0755);
 copy("qmail-pop3d","bin/qmail-pop3d",UID_OWNER,0755);
 copy("qmail-popup","bin/qmail-popup",UID_OWNER,0755);
 copy("qmail-smtpd","bin/qmail-smtpd",UID_OWNER,0755);
 copy("qmail-qmtpd","bin/qmail-qmtpd",UID_OWNER,0755);
 copy("qmail-start","bin/qmail-start",UID_OWNER,0755);
 copy("qmail-showctl","bin/qmail-showctl",UID_OWNER,0755);
 copy("sendmail","bin/sendmail",UID_OWNER,0755);
 copy("mailsubj","bin/mailsubj",UID_OWNER,0755);
 copy("tcp-env","bin/tcp-env",UID_OWNER,0755);
 copy("qlist","bin/qlist",UID_OWNER,0755);
 copy("qlist2","bin/qlist2",UID_OWNER,0755);
 copy("qreceipt","bin/qreceipt",UID_OWNER,0755);
 copy("qsmhook","bin/qsmhook",UID_OWNER,0755);
 copy("preline","bin/preline",UID_OWNER,0755);
 copy("qbiff","bin/qbiff",UID_OWNER,0755);
 copy("forward","bin/forward",UID_OWNER,0755);
 copy("condredirect","bin/condredirect",UID_OWNER,0755);
 copy("maildirmake","bin/maildirmake",UID_OWNER,0755);
 copy("maildir2mbox","bin/maildir2mbox",UID_OWNER,0755);
 copy("maildir2qmtp","bin/maildir2qmtp",UID_OWNER,0755);
 copy("maildir2smtp","bin/maildir2smtp",UID_OWNER,0755);
 copy("maildirwatch","bin/maildirwatch",UID_OWNER,0755);
 copy("elq","bin/elq",UID_OWNER,0755);
 copy("pinq","bin/pinq",UID_OWNER,0755);
 copy("qail","bin/qail",UID_OWNER,0755);

 copy("addresses.0","man/cat5/addresses.0",UID_OWNER,0644);
 copy("envelopes.0","man/cat5/envelopes.0",UID_OWNER,0644);
 copy("forgeries.0","man/cat7/forgeries.0",UID_OWNER,0644);
 copy("forward.0","man/cat1/forward.0",UID_OWNER,0644);
 copy("condredirect.0","man/cat1/condredirect.0",UID_OWNER,0644);
 copy("maildir.0","man/cat5/maildir.0",UID_OWNER,0644);
 copy("mbox.0","man/cat5/mbox.0",UID_OWNER,0644);
 copy("dot-qmail.0","man/cat5/dot-qmail.0",UID_OWNER,0644);
 copy("maildirmake.0","man/cat1/maildirmake.0",UID_OWNER,0644);
 copy("maildir2mbox.0","man/cat1/maildir2mbox.0",UID_OWNER,0644);
 copy("maildir2qmtp.0","man/cat1/maildir2qmtp.0",UID_OWNER,0644);
 copy("maildir2smtp.0","man/cat1/maildir2smtp.0",UID_OWNER,0644);
 copy("maildirwatch.0","man/cat1/maildirwatch.0",UID_OWNER,0644);
 copy("mailsubj.0","man/cat1/mailsubj.0",UID_OWNER,0644);
 copy("qlist.0","man/cat1/qlist.0",UID_OWNER,0644);
 copy("qreceipt.0","man/cat1/qreceipt.0",UID_OWNER,0644);
 copy("qbiff.0","man/cat1/qbiff.0",UID_OWNER,0644);
 copy("preline.0","man/cat1/preline.0",UID_OWNER,0644);
 copy("qmail-alias.0","man/cat8/qmail-alias.0",UID_OWNER,0644);
 copy("qmail-clean.0","man/cat8/qmail-clean.0",UID_OWNER,0644);
 copy("qmail-command.0","man/cat8/qmail-command.0",UID_OWNER,0644);
 copy("qmail-control.0","man/cat5/qmail-control.0",UID_OWNER,0644);
 copy("qmail-header.0","man/cat5/qmail-header.0",UID_OWNER,0644);
 copy("qmail-inject.0","man/cat8/qmail-inject.0",UID_OWNER,0644);
 copy("qmail-limits.0","man/cat7/qmail-limits.0",UID_OWNER,0644);
 copy("qmail-log.0","man/cat5/qmail-log.0",UID_OWNER,0644);
 copy("qmail-lspawn.0","man/cat8/qmail-lspawn.0",UID_OWNER,0644);
 copy("qmail-newuser.0","man/cat8/qmail-newuser.0",UID_OWNER,0644);
 copy("qmail-pw2user.0","man/cat8/qmail-pw2user.0",UID_OWNER,0644);
 copy("qmail-users.0","man/cat5/qmail-users.0",UID_OWNER,0644);
 copy("qmail-getpw.0","man/cat8/qmail-getpw.0",UID_OWNER,0644);
 copy("qmail-queue.0","man/cat8/qmail-queue.0",UID_OWNER,0644);
 copy("qmail-qread.0","man/cat8/qmail-qread.0",UID_OWNER,0644);
 copy("qmail-tcpto.0","man/cat8/qmail-tcpto.0",UID_OWNER,0644);
 copy("qmail-qstat.0","man/cat8/qmail-qstat.0",UID_OWNER,0644);
 copy("qmail-remote.0","man/cat8/qmail-remote.0",UID_OWNER,0644);
 copy("qmail-rspawn.0","man/cat8/qmail-rspawn.0",UID_OWNER,0644);
 copy("qmail-send.0","man/cat8/qmail-send.0",UID_OWNER,0644);
 copy("qmail-pop3d.0","man/cat8/qmail-pop3d.0",UID_OWNER,0644);
 copy("qmail-popup.0","man/cat8/qmail-popup.0",UID_OWNER,0644);
 copy("qmail-smtpd.0","man/cat8/qmail-smtpd.0",UID_OWNER,0644);
 copy("qmail-qmtpd.0","man/cat8/qmail-qmtpd.0",UID_OWNER,0644);
 copy("qmail-start.0","man/cat8/qmail-start.0",UID_OWNER,0644);
 copy("qmail-showctl.0","man/cat8/qmail-showctl.0",UID_OWNER,0644);
 copy("qmail-upgrade.0","man/cat7/qmail-upgrade.0",UID_OWNER,0644);
 copy("qmail.0","man/cat7/qmail.0",UID_OWNER,0644);
 copy("tcp-env.0","man/cat1/tcp-env.0",UID_OWNER,0644);
 copy("tcp-environ.0","man/cat5/tcp-environ.0",UID_OWNER,0644);

 _exit(0);
}
