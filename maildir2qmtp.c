#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "getline.h"
#include "stralloc.h"
#include "substdio.h"
#include "subfd.h"
#include "prioq.h"
#include "signal.h"
#include "fork.h"
#include "readwrite.h"
#include "exit.h"
#include "maildir.h"
#include "open.h"
#include "case.h"
#include "fmt.h"
#include "str.h"
#include "wait.h"
#include "ip.h"
#include "error.h"
#include "timeoutconn.h"
#include "timeoutread.h"
#include "timeoutwrite.h"

#define PORT_QMTP 209

#define FATAL "maildir2qmtp: fatal: "

void die_usage() { strerr_die1x(111,"maildir2qmtp: usage: maildir2qmtp maildir prefix ta.rg.et.ip"); }
void die_qmtpread() { strerr_die2sys(111,FATAL,"network read error: "); }
void die_proto() { strerr_die2x(111,FATAL,"remote protocol violation"); }

char *prefix;
unsigned int prefixlen;
stralloc filenames = {0};
prioq pq = {0};

int fdnet;
int pic2p[2];

char netbuf[1024];
substdio ssnet;
char inbuf[512];
substdio ssin;
char outbuf[128];
substdio ssout;
char tmpbuf[1024];

char num[FMT_ULONG];
char num2[FMT_ULONG];

int match;
stralloc line = {0};
stralloc recipient = {0};
stralloc sender = {0};

void die_qmtpwrite() { _exit(31); }
void die_datafail() { _exit(32); }
void die_pipefail() { _exit(33); }

int succwrite(fd,buf,len) int fd; char *buf; int len;
{
 int w;
 w = timeoutwrite(fd,buf,len);
 if (w <= 0) die_qmtpwrite();
 return w;
}
int succread(fd,buf,len) int fd; char *buf; int len;
{
 int r;
 r = timeoutread(fd,buf,len);
 if (r < 0) die_qmtpread();
 return r;
}

void doit(fn)
char *fn;
{
 struct stat st;
 int fd;
 unsigned long len;
 int len2;
 int i;
 char ch;

 fd = open_read(fn);
 if (fd == -1) return;
 if (fstat(fd,&st) == -1) goto done;
 len = 0;

 substdio_fdbuf(&ssin,read,fd,inbuf,sizeof(inbuf));

 if (getline2(&ssin,&line,&match,'\n') == -1) goto done;
 len += line.len;
 if (!match) goto done;
 if (line.len < 16) goto done;
 if (str_diffn(line.s,"Return-Path: <",14)) goto done;
 if (line.s[line.len - 2] != '>') goto done;
 if (line.s[line.len - 1] != '\n') goto done;
 if (!stralloc_copyb(&sender,line.s + 14,line.len - 16)) goto done;

 if (getline2(&ssin,&line,&match,'\n') == -1) goto done;
 len += line.len;
 if (!match) goto done;
 if (line.len < 15) goto done;
 if (str_diffn(line.s,"Delivered-To: ",14)) goto done;
 if (line.s[line.len - 1] != '\n') goto done;
 if (!stralloc_copyb(&recipient,line.s + 14,line.len - 15)) goto done;

 if (recipient.len < prefixlen) goto done;
 if (str_diffn(recipient.s,prefix,prefixlen)) goto done;

 if (st.st_size < len) goto done; /* okay, who's the wise guy? */
 len = st.st_size + 1 - len;

 if (substdio_putflush(&ssout,fn,str_len(fn) + 1) == -1) die_pipefail();

 substdio_put(&ssnet,num,fmt_ulong(num,len));
 substdio_put(&ssnet,":\n",2);
 --len; /* for the \n */
 while (len > 0)
  {
   /* XXX: could do without tmpbuf here */
   i = len;
   if (i > sizeof(tmpbuf)) i = sizeof(tmpbuf);
   i = substdio_get(&ssin,tmpbuf,i);
   if (i <= 0) die_datafail(); /* wise guy again */
   len -= i;
   substdio_put(&ssnet,tmpbuf,i);
  }
 substdio_put(&ssnet,",",1);
 
 len = sender.len;
 substdio_put(&ssnet,num,fmt_ulong(num,len));
 substdio_put(&ssnet,":",1);
 substdio_put(&ssnet,sender.s,sender.len);
 substdio_put(&ssnet,",",1);

 len = recipient.len;
 len2 = fmt_ulong(num2,len);
 len += len2 + 2;
 substdio_put(&ssnet,num,fmt_ulong(num,len));
 substdio_put(&ssnet,":",1);
 substdio_put(&ssnet,num2,len2);
 substdio_put(&ssnet,":",1);
 substdio_put(&ssnet,recipient.s,recipient.len);
 substdio_put(&ssnet,",,",2);
 substdio_flush(&ssnet);

 done:
 close(fd);
}

void child()
{
 struct prioq_elt pe;

 substdio_fdbuf(&ssnet,succwrite,fdnet,netbuf,sizeof(netbuf));
 substdio_fdbuf(&ssout,write,pic2p[1],outbuf,sizeof(outbuf));

 while (prioq_min(&pq,&pe))
  {
   prioq_delmin(&pq);
   doit(filenames.s + pe.id);
  }
}

void main(argc,argv)
int argc;
char **argv;
{
 struct prioq_elt pe;
 char *maildir;
 char *target;
 struct ip_address targetip;
 int len;
 int pid;
 int wstat;
 char ch;

 signal_init();

 maildir = *++argv; if (!maildir) die_usage();
 prefix = *++argv; if (!prefix) die_usage();
 target = *++argv; if (!target) die_usage();

 prefixlen = str_len(prefix);
 if (target[ip_scan(target,&targetip)]) die_usage();

 if (chdir(maildir) == -1)
   strerr_die4sys(111,FATAL,"unable to chdir to ",maildir,": ");
 maildir_clean(&filenames);

 if (maildir_scan(&pq,&filenames,1,1) == -1)
   strerr_die1(111,FATAL,&maildir_scan_err);

 if (!prioq_min(&pq,&pe)) _exit(0);

 fdnet = socket(AF_INET,SOCK_STREAM,0);
 if (fdnet == -1) strerr_die2sys(111,FATAL,"unable to create socket: ");

 timeoutread_init(600);
 timeoutwrite_init(600);
 if (timeoutconn(fdnet,&targetip,PORT_QMTP,60) == -1)
   strerr_die4sys(111,FATAL,"unable to connect to ",target,": ");

 if (pipe(pic2p) == -1)
   strerr_die2sys(111,FATAL,"unable to create pipe: ");

 pid = fork();
 if (pid == -1) strerr_die2sys(111,FATAL,"unable to fork: ");
 if (pid == 0) { close(pic2p[0]); child(); _exit(0); }
 close(pic2p[1]);

 substdio_fdbuf(&ssnet,succread,fdnet,netbuf,sizeof(netbuf));
 substdio_fdbuf(&ssin,read,pic2p[0],inbuf,sizeof(inbuf));

 for (;;)
  {
   if (getline2(&ssin,&line,&match,'\0') == -1)
     strerr_die2sys(111,FATAL,"unable to read from child: ");
   if (!match)
     break;

   substdio_puts(subfderr,"trying ");
   substdio_puts(subfderr,line.s);
   substdio_puts(subfderr,"...\n");
   substdio_flush(subfderr);

   len = 0;
   for (;;)
    {
     substdio_get(&ssnet,&ch,1);
     if (ch == ':') break;
     if (len > 200000000) die_proto();
     len = 10 * len + (ch - '0');
    }
   if (len == 0) die_proto();
   substdio_get(&ssnet,&ch,1);
   --len;
   substdio_puts(subfderr,"> ");
   substdio_put(subfderr,&ch,1);

   switch(ch)
    {
     default: die_proto();
     case 'Z': break;
     case 'D': break;
     case 'K':
       if (unlink(line.s) == -1)
        {
         substdio_puts(subfderr,"maildir2qmtp: warning: unable to unlink ");
         substdio_puts(subfderr,line.s);
         substdio_puts(subfderr,"; message will be delivered twice\n");
         substdio_flush(subfderr);
        }
    }

   while (len > 0)
    {
     substdio_get(&ssnet,&ch,1);
     if ((ch < 32) || (ch > 126)) ch = '?';
     substdio_put(subfderr,&ch,1);
     --len;
    }
   substdio_puts(subfderr,"\n");
   substdio_flush(subfderr);
   substdio_get(&ssnet,&ch,1);
   if (ch != ',') die_proto();
  }

 if (wait_pid(&wstat,pid) == -1)
   strerr_die2sys(111,FATAL,"unable to get child status: ");
 else
  {
   if (wait_crashed(wstat)) strerr_die2x(111,FATAL,"child crashed");
   switch (wait_exitcode(wstat))
    {
     case 0: break;
     case 31: strerr_die2x(111,FATAL,"network write error"); break;
     case 32: strerr_die2x(111,FATAL,"trouble reading maildir"); break;
     default: strerr_die2x(111,FATAL,"internal bug"); break;
    }
  }
 _exit(0);
}
