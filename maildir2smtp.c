#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "getline.h"
#include "substdio.h"
#include "subfd.h"
#include "stralloc.h"
#include "maildir.h"
#include "prioq.h"
#include "open.h"
#include "readwrite.h"
#include "exit.h"
#include "signal.h"
#include "str.h"
#include "case.h"
#include "error.h"
#include "quote.h"
#include "ip.h"
#include "scan.h"
#include "timeoutconn.h"
#include "timeoutread.h"
#include "timeoutwrite.h"

#define PORT_SMTP 25

#define FATAL "maildir2smtp: fatal: "

void die_smtpwrite() { strerr_die2sys(111,FATAL,"network write error: "); }
void die_smtpread() { strerr_die2sys(111,FATAL,"network read error: "); }
void die_datafail() { strerr_die2sys(111,FATAL,"trouble reading maildir: "); }
void die_usage() { strerr_die1x(111,"maildir2smtp: usage: maildir2smtp maildir prefix ta.rg.et.ip helohost"); }

int flagpipelining = 0;

stralloc dataline = {0};

int blast(ssto,ssfrom)
substdio *ssto; /* neverfail */
substdio *ssfrom;
{
 int match;

 for (;;)
  {
   if (getline2(ssfrom,&dataline,&match,'\n') != 0) return -1;
   if (!match && !dataline.len) break;
   if (match) --dataline.len; /* no way to pass this info over SMTP */
   if (dataline.len && (dataline.s[0] == '.'))
     substdio_put(ssto,".",1);
   substdio_put(ssto,dataline.s,dataline.len);
   substdio_put(ssto,"\r\n",2);
   if (!match) break;
  }
 substdio_put(ssto,".\r\n",3);
 substdio_flush(ssto);
 return 0;
}

char *prefix;
unsigned int prefixlen;

stralloc filenames = {0};
prioq pq = {0};

char inbuf[SUBSTDIO_INSIZE];
substdio ssin;

stralloc line = {0};
stralloc recipient = {0};
stralloc sender = {0};

substdio smtpto;
substdio smtpfrom;
char smtptobuf[1024];
char smtpfrombuf[128];
stralloc smtpline = {0};

int succwrite(fd,buf,len) int fd; char *buf; int len;
{
 int w;
 w = timeoutwrite(fd,buf,len);
 if (w <= 0) die_smtpwrite();
 return w;
}

void printfn(fn) char *fn;
{
 substdio_puts(subfderr,"trying ");
 substdio_puts(subfderr,fn);
 substdio_puts(subfderr,"...\n");
 substdio_flush(subfderr);
}

void printline()
{
 int i;
 char ch;

 substdio_puts(subfderr,"> ");
 for (i = 0;i < smtpline.len - 1;++i)
  {
   ch = smtpline.s[i];
   if ((ch < 32) || (ch > 126)) ch = '?';
   substdio_put(subfderr,&ch,1);
  }
 substdio_puts(subfderr,"\n");
 substdio_flush(subfderr);
}

unsigned long smtpcode(flagehlo)
int flagehlo;
{
 int match;
 unsigned long code;
 int flagfirst;

 flagfirst = 1;
 do
  {
   if (getline2(&smtpfrom,&smtpline,&match,'\n') != 0) die_smtpread();
   if (!match) die_smtpread();
   if ((smtpline.len >= 2) && (smtpline.s[smtpline.len - 2] == '\r'))
    {
     smtpline.s[smtpline.len - 2] = '\n';
     --smtpline.len;
    }
   if (scan_nbblong(smtpline.s,smtpline.len,10,0,&code) != 3) die_smtpread();
   if (code >= 400) printline();
   if (flagehlo)
     if (code == 250)
       if (!flagfirst)
         if (smtpline.len == 15)
	   if (!case_diffb("PIPELINING\n",smtpline.s + 4,11))
	     flagpipelining = 1;
   if (smtpline.len == 3) return code;
   flagfirst = 0;
  }
 while (smtpline.s[3] == '-');

 return code;
}

void quit()
{
 substdio_puts(&smtpto,"QUIT\r\n");
 substdio_flush(&smtpto);
 smtpcode(0); /* protocol design stupidity */
}

stralloc quosender = {0};
stralloc quorecip = {0};

void doit(fn)
char *fn;
{
 int fd;
 int match;

 fd = open_read(fn);
 if (fd == -1) return;
 substdio_fdbuf(&ssin,read,fd,inbuf,sizeof(inbuf));

 printfn(fn);

 if (getline2(&ssin,&line,&match,'\n') == -1) goto done;
 if (!match) goto done;
 if (line.len < 16) goto done;
 if (str_diffn(line.s,"Return-Path: <",14)) goto done;
 if (line.s[line.len - 2] != '>') goto done;
 if (line.s[line.len - 1] != '\n') goto done;
 if (!stralloc_copyb(&sender,line.s + 14,line.len - 16)) goto done;

 if (getline2(&ssin,&line,&match,'\n') == -1) goto done;
 if (!match) goto done;
 if (line.len < 15) goto done;
 if (str_diffn(line.s,"Delivered-To: ",14)) goto done;
 if (line.s[line.len - 1] != '\n') goto done;
 if (!stralloc_copyb(&recipient,line.s + 14,line.len - 15)) goto done;

 if (recipient.len < prefixlen) goto done;
 if (str_diffn(recipient.s,prefix,prefixlen)) goto done;

 if (!stralloc_0(&sender)) goto done;
 if (!quote2(&quosender,sender.s)) goto done;
 if (!stralloc_0(&recipient)) goto done;
 if (!quote2(&quorecip,recipient.s + prefixlen)) goto done;

 substdio_puts(&smtpto,"RSET\r\n");
 if (!flagpipelining) substdio_flush(&smtpto);
 if (!flagpipelining) if (smtpcode(0) >= 400) goto done;

 substdio_puts(&smtpto,"MAIL FROM:<");
 substdio_put(&smtpto,quosender.s,quosender.len);
 substdio_puts(&smtpto,">\r\n");
 if (!flagpipelining) substdio_flush(&smtpto);
 if (!flagpipelining) if (smtpcode(0) >= 400) goto done;

 substdio_puts(&smtpto,"RCPT TO:<");
 substdio_put(&smtpto,quorecip.s,quorecip.len);
 substdio_puts(&smtpto,">\r\n");
 if (!flagpipelining) substdio_flush(&smtpto);
 if (!flagpipelining) if (smtpcode(0) >= 400) goto done;

 substdio_puts(&smtpto,"DATA\r\n");
 substdio_flush(&smtpto);
 if (!flagpipelining) if (smtpcode(0) >= 400) goto done;

 if (flagpipelining)
  {
   int coderset;
   int codemail;
   int codercpt;
   int codedata;

   coderset = smtpcode(0);
   codemail = smtpcode(0);
   codercpt = smtpcode(0);
   codedata = smtpcode(0);
   if (codedata >= 400) goto done;
  }

 if (blast(&smtpto,&ssin) == -1)
   die_datafail(); /* leaves other side unhappy; sigh */

 if (smtpcode(0) >= 400) goto done;
 printline();

 if (unlink(fn) == -1)
  {
   substdio_puts(subfderr,"maildir2smtp: warning: unable to unlink ");
   substdio_puts(subfderr,fn);
   substdio_puts(subfderr,"; message will be delivered twice\n");
   substdio_flush(subfderr);
  }

 done:
 close(fd);
 return;
}

void main(argc,argv)
int argc;
char **argv;
{
 struct prioq_elt pe;
 char *maildir;
 char *target;
 char *helohost;
 struct ip_address targetip;
 int fd;

 signal_init();

 maildir = *++argv; if (!maildir) die_usage();
 prefix = *++argv; if (!prefix) die_usage();
 target = *++argv; if (!target) die_usage();
 helohost = *++argv; if (!helohost) die_usage();

 prefixlen = str_len(prefix);
 if (target[ip_scan(target,&targetip)]) die_usage();

 if (chdir(maildir) == -1)
   strerr_die4sys(111,FATAL,"unable to chdir to ",maildir,": ");
 maildir_clean(&filenames);

 if (maildir_scan(&pq,&filenames,1,1) == -1)
   strerr_die1(111,FATAL,&maildir_scan_err);

 if (!prioq_min(&pq,&pe)) _exit(0);

 fd = socket(AF_INET,SOCK_STREAM,0);
 if (fd == -1) strerr_die2sys(111,FATAL,"unable to create socket: ");

 timeoutread_init(600);
 timeoutwrite_init(600);
 if (timeoutconn(fd,&targetip,PORT_SMTP,60) == -1)
   strerr_die4sys(111,FATAL,"unable to connect to ",target,": ");

 substdio_fdbuf(&smtpto,succwrite,fd,smtptobuf,sizeof(smtptobuf));
 substdio_fdbuf(&smtpfrom,timeoutread,fd,smtpfrombuf,sizeof(smtpfrombuf));

 if (smtpcode(0) != 220)
  {
   quit();
   strerr_die2x(111,FATAL,"connected but greeting failed");
  }

 substdio_puts(&smtpto,"EHLO ");
 substdio_puts(&smtpto,helohost);
 substdio_puts(&smtpto,"\r\n");
 substdio_flush(&smtpto);

 if (smtpcode(1) != 250)
  {
   substdio_puts(&smtpto,"HELO ");
   substdio_puts(&smtpto,helohost);
   substdio_puts(&smtpto,"\r\n");
   substdio_flush(&smtpto);
  
   if (smtpcode(0) != 250)
    {
     quit();
     strerr_die2x(111,FATAL,"connected but my name was rejected");
    }
  }

 while (prioq_min(&pq,&pe))
  {
   prioq_delmin(&pq);
   doit(filenames.s + pe.id);
  }

 quit();
 _exit(0);
}
