#include "substdio.h"
#include "subfd.h"
#include "exit.h"
#include "fmt.h"
#include "control.h"
#include "constmap.h"
#include "stralloc.h"
#include "direntry.h"
#include "conf-home.h"

stralloc me = {0};
int meok;

stralloc line = {0};
char num[FMT_ULONG];

void safeput(buf,len)
char *buf;
unsigned int len;
{
  char ch;

  while (len > 0) {
    ch = *buf;
    if ((ch < 32) || (ch > 126)) ch = '?';
    substdio_put(subfdout,&ch,1);
    ++buf;
    --len;
  }
}

void do_int(fn,def,pre,post)
char *fn;
char *def;
char *pre;
char *post;
{
  int i;
  substdio_puts(subfdout,"\n");
  substdio_puts(subfdout,fn);
  substdio_puts(subfdout,": ");
  switch(control_readint(&i,fn)) {
    case 0:
      substdio_puts(subfdout,"(Default.) ");
      substdio_puts(subfdout,pre);
      substdio_puts(subfdout,def);
      substdio_puts(subfdout,post);
      substdio_puts(subfdout,".\n");
      break;
    case 1:
      if (i < 0) i = 0;
      substdio_puts(subfdout,pre);
      substdio_put(subfdout,num,fmt_uint(num,i));
      substdio_puts(subfdout,post);
      substdio_puts(subfdout,".\n");
      break;
    default:
      substdio_puts(subfdout,"Oops! Trouble reading this file.\n");
      break;
  }
}

void do_str(fn,flagme,def,pre)
char *fn;
int flagme;
char *def;
char *pre;
{
  substdio_puts(subfdout,"\n");
  substdio_puts(subfdout,fn);
  substdio_puts(subfdout,": ");
  switch(control_readline(&line,fn)) {
    case 0:
      substdio_puts(subfdout,"(Default.) ");
      if (!stralloc_copys(&line,def)) {
	substdio_puts(subfdout,"Oops! Out of memory.\n");
	break;
      }
      if (flagme && meok)
	if (!stralloc_copy(&line,&me)) {
	  substdio_puts(subfdout,"Oops! Out of memory.\n");
	  break;
	}
    case 1:
      substdio_puts(subfdout,pre);
      safeput(line.s,line.len);
      substdio_puts(subfdout,".\n");
      break;
    default:
      substdio_puts(subfdout,"Oops! Trouble reading this file.\n");
      break;
  }
}

void do_lst(fn,def,pre,post)
char *fn;
char *def;
char *pre;
char *post;
{
  int i;
  int j;

  substdio_puts(subfdout,"\n");
  substdio_puts(subfdout,fn);
  substdio_puts(subfdout,": ");
  switch(control_readfile(&line,fn)) {
    case 0:
      substdio_puts(subfdout,"(Default.) ");
      substdio_puts(subfdout,def);
      substdio_puts(subfdout,"\n");
      break;
    case 1:
      substdio_puts(subfdout,"\n");
      i = 0;
      for (j = 0;j < line.len;++j)
	if (!line.s[j]) {
          substdio_puts(subfdout,pre);
          safeput(line.s + i,j - i);
          substdio_puts(subfdout,post);
          substdio_puts(subfdout,"\n");
	  i = j + 1;
	}
      break;
    default:
      substdio_puts(subfdout,"Oops! Trouble reading this file.\n");
      break;
  }
}

void main()
{
  DIR *dir;
  direntry *d;

  substdio_puts(subfdout,"The qmail control files are stored in ");
  substdio_puts(subfdout,CONF_HOME);
  substdio_puts(subfdout,"/control.\n");

  if (chdir(CONF_HOME) == -1) {
    substdio_puts(subfdout,"Oops! Unable to chdir to ");
    substdio_puts(subfdout,CONF_HOME);
    substdio_puts(subfdout,".\n");
    substdio_flush(subfdout);
    _exit(111);
  }
  if (chdir("control") == -1) {
    substdio_puts(subfdout,"Oops! Unable to chdir to control.\n");
    substdio_flush(subfdout);
    _exit(111);
  }

  dir = opendir(".");
  if (!dir) {
    substdio_puts(subfdout,"Oops! Unable to open current directory.\n");
    substdio_flush(subfdout);
    _exit(111);
  }

  meok = control_readline(&me,"me");
  if (meok == -1) {
    substdio_puts(subfdout,"Oops! Trouble reading control/me.");
    substdio_flush(subfdout);
    _exit(111);
  }

  do_lst("badmailfrom","Any MAIL FROM is allowed.",""," not accepted in MAIL FROM.");
  do_str("bouncefrom",0,"MAILER-DAEMON","Bounce user name is ");
  do_str("bouncehost",1,"bouncehost","Bounce host name is ");
  do_int("concurrencylocal","10","Local concurrency is ","");
  do_int("concurrencyremote","20","Remote concurrency is ","");
  do_str("defaultdomain",1,"defaultdomain","Default domain name is ");
  do_str("defaulthost",1,"defaulthost","Default host name is ");
  do_str("doublebouncehost",1,"doublebouncehost","2B recipient host: ");
  do_str("doublebounceto",0,"postmaster","2B recipient user: ");
  do_str("envnoathost",1,"envnoathost","Presumed domain name is ");
  do_str("helohost",1,"helohost","SMTP client HELO host name is ");
  do_str("idhost",1,"idhost","Message-ID host name is ");
  do_str("localiphost",1,"localiphost","Local IP address becomes ");
  do_lst("locals","Messages for me are delivered locally.","Messages for "," are delivered locally.");
  do_str("me",0,"undefined! Uh-oh","My name is ");
  do_lst("percenthack","The percent hack is not allowed.","The percent hack is allowed for user%host@",".");
  do_str("plusdomain",1,"plusdomain","Plus domain name is ");
  do_int("queuelifetime","604800","Message lifetime in the queue is "," seconds");
  do_lst("rcpthosts","SMTP clients may send messages to any recipient.","SMTP clients may send messages to recipients at ",".");
  do_str("smtpgreeting",1,"smtpgreeting","SMTP greeting: 220 ");
  do_lst("smtproutes","No artificial SMTP routes.","SMTP route: ","");
  do_int("timeoutconnect","60","SMTP client connection timeout is "," seconds");
  do_int("timeoutremote","1200","SMTP client data timeout is "," seconds");
  do_int("timeoutsmtpd","1200","SMTP server data timeout is "," seconds");
  do_lst("virtualdomains","No virtual domains.","Virtual domain: ","");

  while (d = readdir(dir)) {
    if (!str_diff(d->d_name,".")) continue;
    if (!str_diff(d->d_name,"..")) continue;
    if (!str_diff(d->d_name,"bouncefrom")) continue;
    if (!str_diff(d->d_name,"bouncehost")) continue;
    if (!str_diff(d->d_name,"badmailfrom")) continue;
    if (!str_diff(d->d_name,"bouncefrom")) continue;
    if (!str_diff(d->d_name,"bouncehost")) continue;
    if (!str_diff(d->d_name,"concurrencylocal")) continue;
    if (!str_diff(d->d_name,"concurrencyremote")) continue;
    if (!str_diff(d->d_name,"defaultdomain")) continue;
    if (!str_diff(d->d_name,"defaulthost")) continue;
    if (!str_diff(d->d_name,"doublebouncehost")) continue;
    if (!str_diff(d->d_name,"doublebounceto")) continue;
    if (!str_diff(d->d_name,"envnoathost")) continue;
    if (!str_diff(d->d_name,"helohost")) continue;
    if (!str_diff(d->d_name,"idhost")) continue;
    if (!str_diff(d->d_name,"localiphost")) continue;
    if (!str_diff(d->d_name,"locals")) continue;
    if (!str_diff(d->d_name,"me")) continue;
    if (!str_diff(d->d_name,"percenthack")) continue;
    if (!str_diff(d->d_name,"plusdomain")) continue;
    if (!str_diff(d->d_name,"queuelifetime")) continue;
    if (!str_diff(d->d_name,"rcpthosts")) continue;
    if (!str_diff(d->d_name,"smtpgreeting")) continue;
    if (!str_diff(d->d_name,"smtproutes")) continue;
    if (!str_diff(d->d_name,"timeoutconnect")) continue;
    if (!str_diff(d->d_name,"timeoutremote")) continue;
    if (!str_diff(d->d_name,"timeoutsmtpd")) continue;
    if (!str_diff(d->d_name,"virtualdomains")) continue;
    substdio_puts(subfdout,"\n");
    substdio_puts(subfdout,d->d_name);
    substdio_puts(subfdout,": I have no idea what this file does.\n");
  }

  substdio_flush(subfdout);
  _exit(0);
}
