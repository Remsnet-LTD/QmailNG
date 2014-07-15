#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "substdio.h"
#include "getln.h"
#include "readwrite.h"
#include "exit.h"
#include "stralloc.h"
#include "slurpclose.h"
#include "error.h"
#include "sig.h"
#include "ip.h"
#include "timeoutconn.h"
#include "timeoutread.h"
#include "timeoutwrite.h"
#include "auto_qmail.h"
#include "control.h"
#include "fmt.h"

#ifndef PORT_QMQP /* this is for testing purposes, so you can overwrite
		     this port via a simple -D argument */
#define PORT_QMQP 628
#endif

void die_success() { _exit(0); }
void die_perm() { _exit(31); }
void nomem() { _exit(51); }
void die_read() { if (errno == error_nomem) nomem(); _exit(54); }
void die_control() { _exit(55); }
void die_socket() { _exit(56); }
void die_home() { _exit(61); }
void die_temp() { _exit(71); }
void die_conn() { _exit(74); }
void die_format() { _exit(91); }

int lasterror = 55;
int qmqpfd;

int saferead(fd,buf,len) int fd; char *buf; int len;
{
  int r;
  r = timeoutread(60,qmqpfd,buf,len);
  if (r <= 0) die_conn();
  return r;
}
int safewrite(fd,buf,len) int fd; char *buf; int len;
{
  int r;
  r = timeoutwrite(60,qmqpfd,buf,len);
  if (r <= 0) die_conn();
  return r;
}

char buf[1024];
substdio to = SUBSTDIO_FDBUF(safewrite,-1,buf,sizeof buf);
substdio from = SUBSTDIO_FDBUF(saferead,-1,buf,sizeof buf);
substdio envelope = SUBSTDIO_FDBUF(read,1,buf,sizeof buf);
/* WARNING: can use only one of these at a time! */

stralloc beforemessage = {0};
stralloc toline = {0};
stralloc dtline = {0};
stralloc message = {0};
stralloc aftermessage = {0};

char strnum[FMT_ULONG];
stralloc line = {0};

void getmess()
{
  int match;
  int i;

  if (slurpclose(0,&message,1024) == -1) die_read();

  strnum[fmt_ulong(strnum,(unsigned long) message.len)] = 0;
  if (!stralloc_copys(&beforemessage,strnum)) nomem();
  if (!stralloc_cats(&beforemessage,":")) nomem();
  if (!stralloc_copys(&aftermessage,",")) nomem();

  if (getln(&envelope,&line,&match,'\0') == -1) die_read();
  if (!match) die_format();
  if (line.len < 2) die_format();
  if (line.s[0] != 'F') die_format();

  strnum[fmt_ulong(strnum,(unsigned long) line.len - 2)] = 0;
  if (!stralloc_cats(&aftermessage,strnum)) nomem();
  if (!stralloc_cats(&aftermessage,":")) nomem();
  if (!stralloc_catb(&aftermessage,line.s + 1,line.len - 2)) nomem();
  if (!stralloc_cats(&aftermessage,",")) nomem();

  for (;;) {
    if (getln(&envelope,&line,&match,'\0') == -1) die_read();
    if (!match) die_format();
    if (line.len < 2) break;
    if (line.s[0] != 'T') die_format();

    strnum[fmt_ulong(strnum,(unsigned long) line.len - 2)] = 0;
    if (!stralloc_cats(&aftermessage,strnum)) nomem();
    if (!stralloc_cats(&aftermessage,":")) nomem();
    if (!stralloc_catb(&aftermessage,line.s + 1,line.len - 2)) nomem();
    if (!stralloc_cats(&aftermessage,",")) nomem();
    /* only use the last (and only) TO address */
    if (!stralloc_copyb(&toline,line.s + 1,line.len - 2)) nomem();
  }
  /* extension to qmail-qmqpc for clustering mode, add Delivered-To: CLUSTERHOST $HOST *
   * at the top of the mail */
  if (getln(&envelope,&line,&match,'\0') == -1) die_read();
  if (!match) {
    dtline.len = 0; /* just set the lenght to 0 */
    dtline.s = 0; /* just to be sure */
    return;
  }
  if (line.len < 2) die_format();
  if (line.s[0] != 'H') die_format();
  if (!stralloc_copys(&dtline, "Delivered-To: CLUSTERHOST ")) nomem();
  if (!stralloc_catb(&dtline, line.s + 1,line.len - 2 )) nomem();
  if (!stralloc_cats(&dtline, " ")) nomem();
  if (!stralloc_cat(&dtline, &toline)) nomem();
  for (i = 0;i < dtline.len;++i) if (dtline.s[i] == '\n') dtline.s[i] = '_';
  if (!stralloc_cats(&dtline,"\n")) nomem();
  strnum[fmt_ulong(strnum,(unsigned long) message.len+dtline.len)] = 0;
  if (!stralloc_copys(&beforemessage,strnum)) nomem();
  if (!stralloc_cats(&beforemessage,":")) nomem();

}

struct ip_address outip;

void doit(server)
char *server;
{
  struct ip_address ip;
  char ch;

  if (!ip_scan(server,&ip)) return;

  qmqpfd = socket(AF_INET,SOCK_STREAM,0);
  if (qmqpfd == -1) die_socket();

  if (timeoutconn(qmqpfd,&ip,&outip,PORT_QMQP,10) != 0) {
    lasterror = 73;
    if (errno == error_timeout) lasterror = 72;
    close(qmqpfd);
    return;
  }

  strnum[fmt_ulong(strnum, (unsigned long)
         (beforemessage.len + dtline.len + message.len + aftermessage.len))] = 0;
  substdio_puts(&to,strnum);
  substdio_puts(&to,":");
  substdio_put(&to,beforemessage.s,beforemessage.len);
  substdio_put(&to,dtline.s,dtline.len);
  substdio_put(&to,message.s,message.len);
  substdio_put(&to,aftermessage.s,aftermessage.len);
  substdio_puts(&to,",");
  substdio_flush(&to);

  for (;;) {
    substdio_get(&from,&ch,1);
    if (ch == 'K') die_success();
    if (ch == 'Z') die_temp();
    if (ch == 'D') die_perm();
  }
}

stralloc servers = {0};
stralloc outgoingip = {0};

#include "dns.h"
#include "ipalloc.h"

ipalloc ia = {0};

void main(argc,argv)
int argc;
char **argv;
{
  int i;
  int j;

  sig_pipeignore();

  if (chdir(auto_qmail) == -1) die_home();
  if (control_init() == -1) die_control();
  if ( argv[1] ) {
    char temp[IPFMT];
    if (!stralloc_copys(&servers,argv[1])) nomem();
    dns_init(0);
    switch (dns_ip(&ia,&servers)) {
      case DNS_HARD: die_perm();
      case DNS_SOFT: die_temp();
      case DNS_MEM: nomem();
    }

    temp[ip_fmt(temp,&ia.ix[0].ip)]=0;
    if (!stralloc_copys(&servers, temp)) nomem();
    if (!stralloc_0(&servers)) nomem();
  } else
  if (control_readfile(&servers,"control/qmqpservers",0) != 1) die_control();

  if (control_rldef(&outgoingip, "control/qmqpcip", 0, "0.0.0.0") == -1)
	  die_control();
  if (!stralloc_0(&outgoingip)) nomem();
  if (ip_scan(outgoingip.s,&outip)) die_control();

  getmess();

  i = 0;
  for (j = 0;j < servers.len;++j)
    if (!servers.s[j]) {
      doit(servers.s + i);
      i = j + 1;
    }

  _exit(lasterror);
}
