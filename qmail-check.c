#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include "subfd.h"
#include "substdio.h"
#include "open.h"
#include "fmt.h"
#include "error.h"
#include "conf-home.h"
#include "conf-unusual.h"
#include "auto-uids.h"
#include "exit.h"

void warn(s1,s2,s3,s4) char *s1; char *s2; char *s3; char *s4;
{
 substdio_puts(subfderr,"qmail-check: warning: ");
 substdio_puts(subfderr,s1);
 substdio_puts(subfderr,s2);
 substdio_puts(subfderr,s3);
 substdio_puts(subfderr,s4);
 substdio_putsflush(subfderr,"\n");
}

void check(fn,type,uid,mode)
char *fn;
int type;
int uid;
int mode;
{
 struct stat st;
 if (stat(fn,&st) != -1)
  {
   if (st.st_uid != uid)
     warn(CONF_HOME,"/",fn," has wrong owner");
   if ((st.st_mode & S_IFMT) != type)
     warn(CONF_HOME,"/",fn," has wrong type");
   if ((st.st_mode & mode) != mode)
     warn(CONF_HOME,"/",fn," does not allow enough access");
   if (!(mode & 020))
     if (st.st_mode & 020)
       warn(CONF_HOME,"/",fn," is writable to group");
   if (!(mode & 002))
     if (st.st_mode & 002)
       warn(CONF_HOME,"/",fn," is writable to others");
   if (!(mode & 040))
     if (st.st_mode & 040)
       warn(CONF_HOME,"/",fn," is readable to group");
   if (!(mode & 004))
     if (st.st_mode & 004)
       warn(CONF_HOME,"/",fn," is readable to others");
   return;
  }
 if (errno == error_noent)
   warn(CONF_HOME,"/",fn," does not exist");
 else
   warn("unable to stat ",CONF_HOME,"/",fn);
}

void checksplit(fn,type,uid,mode)
char *fn;
int type;
int uid;
int mode;
{
 char foo[40]; /*XXX*/
 unsigned int len;
 int i;
 check(fn,type,uid,mode);
 for (i = 0;i < QFN_SPLIT;++i)
  {
   len = 0;
   len += fmt_str(foo + len,fn);
   foo[len++] = '/';
   len += fmt_ulong(foo + len,(unsigned long) i);
   foo[len] = 0;
   check(foo,type,uid,mode);
  }
}

void main()
{
 if (chdir(CONF_HOME) == -1)
  {
   substdio_putflush(subfderr,"qmail-check: fatal: unable to switch to home directory\n");
   _exit(111);
  }

 check(".",S_IFDIR,UID_OWNER,0755);
 check("control",S_IFDIR,UID_OWNER,0755);
 check("users",S_IFDIR,UID_OWNER,0755);
 check("queue",S_IFDIR,UID_QUEUE,0750);
 check("bin",S_IFDIR,UID_OWNER,0755);
 check("alias",S_IFDIR,UID_ALIAS,0755);
 check("man",S_IFDIR,UID_OWNER,0755);
 check("man/cat1",S_IFDIR,UID_OWNER,0755);
 check("man/cat5",S_IFDIR,UID_OWNER,0755);
 check("man/cat7",S_IFDIR,UID_OWNER,0755);
 check("man/cat8",S_IFDIR,UID_OWNER,0755);

 check("queue/pid",S_IFDIR,UID_QUEUE,0700);
 check("queue/intd",S_IFDIR,UID_QUEUE,0700);
 checksplit("queue/mess",S_IFDIR,UID_QUEUE,0750);
 check("queue/todo",S_IFDIR,UID_QUEUE,0750);
 checksplit("queue/info",S_IFDIR,UID_SEND,0700);
 checksplit("queue/local",S_IFDIR,UID_SEND,0700);
 checksplit("queue/remote",S_IFDIR,UID_SEND,0700);
 check("queue/bounce",S_IFDIR,UID_SEND,0700);
 check("queue/lock",S_IFDIR,UID_QUEUE,0750);

 check("queue/lock/trigger",S_IFIFO,UID_SEND,0622);
 check("queue/lock/sendmutex",S_IFREG,UID_SEND,0600);
 check("queue/lock/tcpto",S_IFREG,UID_REMOTE,0644);

 check("bin/qmail-alias",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-clean",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-home",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-inject",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-lspawn",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-newu",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-pw2u",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-getpw",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-qread",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-tcpto",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-qstat",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-queue",S_IFREG,UID_QUEUE,04755);
 check("bin/qmail-remote",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-rspawn",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-send",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-showctl",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-pop3d",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-popup",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-smtpd",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-qmtpd",S_IFREG,UID_OWNER,0755);
 check("bin/qmail-start",S_IFREG,UID_OWNER,0755);
 check("bin/sendmail",S_IFREG,UID_OWNER,0755);
 check("bin/predate",S_IFREG,UID_OWNER,0755);
 check("bin/datemail",S_IFREG,UID_OWNER,0755);
 check("bin/mailsubj",S_IFREG,UID_OWNER,0755);
 check("bin/splogger",S_IFREG,UID_OWNER,0755);
 check("bin/tcp-env",S_IFREG,UID_OWNER,0755);
 check("bin/qlist",S_IFREG,UID_OWNER,0755);
 check("bin/qlist2",S_IFREG,UID_OWNER,0755);
 check("bin/qreceipt",S_IFREG,UID_OWNER,0755);
 check("bin/qsmhook",S_IFREG,UID_OWNER,0755);
 check("bin/preline",S_IFREG,UID_OWNER,0755);
 check("bin/qbiff",S_IFREG,UID_OWNER,0755);
 check("bin/forward",S_IFREG,UID_OWNER,0755);
 check("bin/condredirect",S_IFREG,UID_OWNER,0755);
 check("bin/maildirmake",S_IFREG,UID_OWNER,0755);
 check("bin/maildir2mbox",S_IFREG,UID_OWNER,0755);
 check("bin/maildirwatch",S_IFREG,UID_OWNER,0755);
 check("bin/elq",S_IFREG,UID_OWNER,0755);
 check("bin/qail",S_IFREG,UID_OWNER,0755);
 check("bin/pinq",S_IFREG,UID_OWNER,0755);

 check("man/cat5/addresses.0",S_IFREG,UID_OWNER,0644);
 check("man/cat5/envelopes.0",S_IFREG,UID_OWNER,0644);
 check("man/cat7/forgeries.0",S_IFREG,UID_OWNER,0644);
 check("man/cat1/forward.0",S_IFREG,UID_OWNER,0644);
 check("man/cat1/condredirect.0",S_IFREG,UID_OWNER,0644);
 check("man/cat5/maildir.0",S_IFREG,UID_OWNER,0644);
 check("man/cat5/mbox.0",S_IFREG,UID_OWNER,0644);
 check("man/cat5/dot-qmail.0",S_IFREG,UID_OWNER,0644);
 check("man/cat1/maildirmake.0",S_IFREG,UID_OWNER,0644);
 check("man/cat1/maildir2mbox.0",S_IFREG,UID_OWNER,0644);
 check("man/cat1/maildirwatch.0",S_IFREG,UID_OWNER,0644);
 check("man/cat1/mailsubj.0",S_IFREG,UID_OWNER,0644);
 check("man/cat1/qlist.0",S_IFREG,UID_OWNER,0644);
 check("man/cat1/qbiff.0",S_IFREG,UID_OWNER,0644);
 check("man/cat1/preline.0",S_IFREG,UID_OWNER,0644);
 check("man/cat1/qreceipt.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-alias.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-clean.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-command.0",S_IFREG,UID_OWNER,0644);
 check("man/cat5/qmail-control.0",S_IFREG,UID_OWNER,0644);
 check("man/cat5/qmail-header.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-inject.0",S_IFREG,UID_OWNER,0644);
 check("man/cat7/qmail-limits.0",S_IFREG,UID_OWNER,0644);
 check("man/cat5/qmail-log.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-lspawn.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-newu.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-pw2u.0",S_IFREG,UID_OWNER,0644);
 check("man/cat5/qmail-users.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-getpw.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-queue.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-qread.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-tcpto.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-qstat.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-remote.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-rspawn.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-send.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-pop3d.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-popup.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-smtpd.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-qmtpd.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-start.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/qmail-showctl.0",S_IFREG,UID_OWNER,0644);
 check("man/cat8/splogger.0",S_IFREG,UID_OWNER,0644);
 check("man/cat7/qmail-upgrade.0",S_IFREG,UID_OWNER,0644);
 check("man/cat7/qmail.0",S_IFREG,UID_OWNER,0644);
 check("man/cat1/tcp-env.0",S_IFREG,UID_OWNER,0644);
 check("man/cat5/tcp-environ.0",S_IFREG,UID_OWNER,0644);

 _exit(0);
}
