#include "signal.h"
#include "readwrite.h"
#include "getline.h"
#include "stralloc.h"
#include "substdio.h"
#include "alloc.h"
#include "conf-home.h"
#include "control.h"
#include "newfield.h"
#include "constmap.h"
#include "error.h"
#include "ipme.h"
#include "ip.h"
#include "qqtalk.h"
#include "str.h"
#include "fmt.h"
#include "byte.h"
#include "case.h"
#include "env.h"
#include "now.h"
#include "exit.h"

#define MAXHOPS 100
int timeout = 1200;

char ssoutbuf[512];
substdio ssout = SUBSTDIO_FDBUF(write,1,ssoutbuf,sizeof(ssoutbuf));

void die() { substdio_flush(&ssout); _exit(1); }
void flush() { if (substdio_flush(&ssout) == -1) _exit(1); }
void out(s) char *s; { if (substdio_puts(&ssout,s) == -1) die(); }

int timeoutread(fd,buf,n) int fd; char *buf; int n;
{
 int r; int saveerrno;
 flush();
 alarm(timeout);
 r = read(fd,buf,n); saveerrno = errno;
 alarm(0);
 errno = saveerrno; return r;
}

char ssinbuf[1024];
substdio ssin = SUBSTDIO_FDBUF(timeoutread,0,ssinbuf,sizeof(ssinbuf));


void outofmem() { out("421 out of memory (#4.3.0)\r\n"); die(); }
void sigalrm() { out("451 timeout (#4.4.2)\r\n"); die(); }

struct qqtalk qqt;
stralloc greeting = {0};
int liphostok = 0;
stralloc liphost = {0};
int rhok = 0;
stralloc rcpthosts = {0};
struct constmap maprcpthosts;

stralloc helohost = {0};
stralloc mailfrom = {0};
stralloc rcptto = {0};
int seenhelo = 0;
int seenmail = 0;

stralloc addr = {0}; /* will be 0-terminated, if addrparse returns 1 */

char *remoteip;
char *remotehost;
char *remoteinfo;
char *localip;
int localiplen;
char *localhost;
int localhostlen;
char *relayclient;

void getenvs()
{
 remoteip = env_get("TCPREMOTEIP");
 if (!remoteip) die();
 localip = env_get("TCPLOCALIP");
 if (!localip) die();
 localhost = env_get("TCPLOCALHOST");
 if (localhost) localhostlen = str_len(localhost);
 remotehost = env_get("TCPREMOTEHOST");
 remoteinfo = env_get("TCPREMOTEINFO");
 relayclient = env_get("RELAYCLIENT");
}

void dohelo(arg) char *arg;
{
 char *real;
 char *myname;
 seenhelo = 1;
 real = remotehost;
 if (!real) real = "unknown";
 if (!arg) arg = real;
 if (!stralloc_copys(&helohost,arg)) outofmem(); 
 if (!stralloc_0(&helohost)) outofmem(); 
 localiplen = str_len(localip);
 if (!case_diffs(real,helohost.s)) real = 0;
 myname = localhost ? localhost : localip;
 if (!newfield_recmake(helohost.s,real,remoteinfo,remoteip,myname,now()))
   outofmem();
}

void blast(ssfrom,hops)
substdio *ssfrom;
int *hops;
{
 char ch;
 int state;
 int flaginheader;
 int pos; /* number of bytes since most recent \n, if fih */
 int flagmaybex; /* 1 if this line might match RECEIVED, if fih */
 int flagmaybey; /* 1 if this line might match \r\n, if fih */
 int flagmaybez; /* 1 if this line might match DELIVERED, if fih */

 state = 1;
 *hops = 0;
 flaginheader = 1;
 pos = 0; flagmaybex = flagmaybey = flagmaybez = 1;
 for (;;)
  {
   if (substdio_get(ssfrom,&ch,1) <= 0) die();
   if (flaginheader)
    {
     if (pos < 9)
      {
       if (ch != "delivered"[pos]) if (ch != "DELIVERED"[pos]) flagmaybez = 0;
       if (flagmaybez) if (pos == 8) ++*hops;
       if (pos < 8)
         if (ch != "received"[pos]) if (ch != "RECEIVED"[pos]) flagmaybex = 0;
       if (flagmaybex) if (pos == 7) ++*hops;
       if (pos < 2) if (ch != "\r\n"[pos]) flagmaybey = 0;
       if (flagmaybey) if (pos == 1) flaginheader = 0;
      }
     ++pos;
     if (ch == '\n') { pos = 0; flagmaybex = flagmaybey = flagmaybez = 1; }
    }
   switch(state)
    {
     case 0:
       if (ch == '\r') { state = 4; continue; }
       break;
     case 1: /* \r\n */
       if (ch == '.') { state = 2; continue; }
       if (ch == '\r') { state = 4; continue; }
       state = 0;
       break;
     case 2: /* \r\n + . */
       if (ch == '\r') { state = 3; continue; }
       state = 0;
       break;
     case 3: /* \r\n + .\r */
       if (ch == '\n') return;
       qqtalk_put(&qqt,".\r",2);
       if (ch == '\r') { state = 4; continue; }
       state = 0;
       break;
     case 4: /* + \r */
       if (ch == '\n') { state = 1; break; }
       if (ch != '\r') { qqtalk_put(&qqt,"\r",1); state = 0; }
    }
   qqtalk_put(&qqt,&ch,1);
  }
}

int addrparse(arg)
char *arg;
{
 int i;
 char ch;
 struct ip_address ip;
 int flagesc;
 int flagquoted;

 arg += str_chr(arg,'<');
 if (*arg != '<') return 0;
 ++arg;

 /* strip source route */
 if (*arg == '@') while (*arg) if (*arg++ == ':') break;

 if (!*arg) return 0;
 if (!stralloc_copys(&addr,"")) outofmem();
 flagesc = 0;
 flagquoted = 0;
 for (i = 0;ch = arg[i];++i) /* copy arg to addr, stripping quotes */
  {
   if (flagesc)
    { if (!stralloc_append(&addr,&ch)) outofmem(); flagesc = 0; }
   else
    {
     if (!flagquoted && (ch == '>')) break;
     switch(ch)
      {
       case '\\': flagesc = 1; break;
       case '"': flagquoted = !flagquoted; break;
       default: if (!stralloc_append(&addr,&ch)) outofmem();
      }
    }
  }
 if (!ch) return 0;
 if (!stralloc_append(&addr,"")) outofmem();
 ++i;
 while (arg[i])
  {
   if (!case_diffs(arg + i," BODY=8BITMIME")) i += 14;
   else if (!case_diffs(arg + i," BODY=7BIT")) i += 10;
   else return 0;
  }

 if (liphostok)
  {
   i = byte_rchr(addr.s,addr.len,'@');
   if (i < addr.len) /* if not, partner should go read rfc 821 */
     if (addr.s[i + 1] == '[')
       if (!addr.s[i + 1 + ip_scanbracket(addr.s + i + 1,&ip)])
         if (ipme_is(&ip))
          {
           addr.len = i + 1;
           if (!stralloc_cat(&addr,&liphost)) outofmem();
           if (!stralloc_0(&addr)) outofmem();
          }
  }

 return 1;
}

int addrallowed()
{
 int j;
 if (!rhok) return 1;
 j = byte_rchr(addr.s,addr.len,'@');
 if (j >= addr.len) return 1; /* can be taken care of by envnoathost */
 if (constmap(&maprcpthosts,addr.s + j + 1,addr.len - j - 2)) return 1;
 for (;j < addr.len;++j)
   if (addr.s[j] == '.')
     if (constmap(&maprcpthosts,addr.s + j,addr.len - j - 1)) return 1;
 return 0;
}

void smtp_greet(code) char *code; {
 if (substdio_puts(&ssout,code) == -1) die();
 if (substdio_put(&ssout,greeting.s,greeting.len) == -1) die(); }
void smtp_quit() { smtp_greet("221 "); out("\r\n"); die(); }
void smtp_help() { out("214-qmail home page: http://pobox.com/~djb/qmail.html\r\n214 send comments to qmail@pobox.com\r\n"); }
void err_syntax() { out("555 syntax error (#5.5.4)\r\n"); }
void err_nogateway() { out("553 sorry, that domain isn't in my list of allowed rcpthosts (#5.7.1)\r\n"); }
void err_unimpl() { out("502 unimplemented (#5.5.1)\r\n"); }
void err_seenmail() { out("503 one MAIL per message (#5.5.1)\r\n"); }
void err_wantmail() { out("503 MAIL first (#5.5.1)\r\n"); }
void err_wantrcpt() { out("503 RCPT first (#5.5.1)\r\n"); }
void err_noop() { out("250 ok\r\n"); }
void err_vrfy() { out("252 send some mail, i'll try my best\r\n"); }
void err_qqt() { out("451 qqt failure (#4.3.0)\r\n"); }
void smtp_helo(arg) char *arg; {
 if (!arg) arg = "";
 smtp_greet("250-"); out("\r\n250-PIPELINING\r\n250 8BITMIME\r\n");
 seenmail = 0;
 dohelo(arg); }
void smtp_rset() {
 if (!seenhelo) dohelo((char *) 0);
 seenmail = 0;
 out("250 flushed\r\n"); }
void smtp_mail(arg) char *arg; {
 if (!seenhelo) dohelo((char *) 0);
 if (seenmail) { err_seenmail(); return; }
 if (!arg) { err_syntax(); return; }
 if (!addrparse(arg)) { err_syntax(); return; }
 seenmail = 1; out("250 ok\r\n");
 if (!stralloc_copys(&rcptto,"")) outofmem();
 if (!stralloc_copys(&mailfrom,addr.s)) outofmem();
 if (!stralloc_0(&mailfrom)) outofmem(); }
void smtp_rcpt(arg) char *arg; {
 if (!seenhelo) dohelo((char *) 0);
 if (!seenmail) { err_wantmail(); return; }
 if (!arg) { err_syntax(); return; }
 if (!addrparse(arg)) { err_syntax(); return; }
 if (relayclient)
  {
   --addr.len;
   if (!stralloc_cats(&addr,relayclient)) outofmem();
   if (!stralloc_0(&addr)) outofmem();
  }
 else
   if (!addrallowed()) { err_nogateway(); return; }
 out("250 ok\r\n");
 if (!stralloc_cats(&rcptto,"T")) outofmem();
 if (!stralloc_cats(&rcptto,addr.s)) outofmem();
 if (!stralloc_0(&rcptto)) outofmem(); }

char accept_buf[FMT_ULONG];
void acceptmessage(qp) unsigned long qp;
{
 datetime_sec when;
 when = now();
 out("250 ok ");
 accept_buf[fmt_ulong(accept_buf,(unsigned long) when)] = 0;
 out(accept_buf);
 out(" qp ");
 accept_buf[fmt_ulong(accept_buf,qp)] = 0;
 out(accept_buf);
 out("\r\n");
}

void smtp_data() {
 int hops; int r; unsigned long qp;
 if (!seenhelo) dohelo((char *) 0);
 if (!seenmail) { err_wantmail(); return; }
 if (!rcptto.len) { err_wantrcpt(); return; }
 seenmail = 0;
 if (qqtalk_open(&qqt,0) == -1) { err_qqt(); return; }
 qp = qqtalk_qp(&qqt);
 out("354 go ahead\r\n");

 qqtalk_puts(&qqt,newfield_rec);
 blast(&ssin,&hops);
 hops = (hops >= MAXHOPS);
 if (hops) qqtalk_fail(&qqt);
 qqtalk_from(&qqt,mailfrom.s);
 qqtalk_eput(&qqt,rcptto.s,rcptto.len);

 r = qqtalk_close(&qqt);
 if (!r) { acceptmessage(qp); return; }
 if (hops) { out("554 too many hops, this message is looping (#5.4.6)\r\n"); return; }
 switch(r)
  {
   case QQT_TOOLONG: out("554 address too long (#5.1.3)\r\n"); return;
   case QQT_EXECHARD: out("554 could not exec qq (#5.3.5)\r\n"); return;
   case QQT_SYS: out("451 qq system error (#4.3.0)\r\n"); return;
   case QQT_READ: out("451 qq read error (#4.3.0)\r\n"); return;
   case QQT_WRITE: out("451 qq write error or disk full (#4.3.0)\r\n"); return;
   case QQT_NOMEM: out("451 qq out of memory (#4.3.0)\r\n"); return;
   case QQT_EXECSOFT: out("451 could not exec qq (#4.3.0)\r\n"); return;
   case QQT_TIMEOUT: out("451 qq timeout (#4.3.0)\r\n"); return;
   case QQT_WAITPID: out("451 qq waitpid surprise (#4.3.0)\r\n"); return;
   case QQT_CRASHED: out("451 qq crashed (#4.3.0)\r\n"); return;
   case QQT_USAGE: out("451 qq usage surprise (#4.3.0)\r\n"); return;
   default: out("451 qq internal bug (#4.3.0)\r\n"); return;
  }
}

static struct { void (*fun)(); char *text; int flagflush; } smtpcmd[] = {
  { smtp_rcpt, "rcpt", 0 }
, { smtp_mail, "mail", 0 }
, { smtp_data, "data", 1 }
, { smtp_quit, "quit", 1 }
, { smtp_helo, "helo", 1 }
, { smtp_helo, "ehlo", 1 }
, { smtp_rset, "rset", 0 }
, { smtp_help, "help", 1 }
, { err_noop, "noop", 1 }
, { err_vrfy, "vrfy", 1 }
, { 0, 0, 0 }
};

void doit(cmd)
char *cmd;
{
 int i;
 int j;
 char ch;

 for (i = 0;smtpcmd[i].fun;++i)
  {
   for (j = 0;ch = smtpcmd[i].text[j];++j)
     if ((cmd[j] != ch) && (cmd[j] != ch - 32))
       break;
   if (!ch)
     if (!cmd[j] || (cmd[j] == ' '))
      {
       while (cmd[j] == ' ') ++j;
       if (!cmd[j])
         smtpcmd[i].fun((char *) 0);
       else
         smtpcmd[i].fun(cmd + j);
       if (smtpcmd[i].flagflush) flush();
       return;
      }
  }
 err_unimpl();
 flush();
}

void getcontrols()
{
 if (control_init() == -1) die();
 if (control_rldef(&greeting,"control/smtpgreeting",1,(char *) 0) != 1) die();
 switch(control_rldef(&liphost,"control/localiphost",1,(char *) 0))
  { case -1: die(); case 1: liphostok = 1; }
 if (control_readint(&timeout,"control/timeoutsmtpd") == -1) die();
 if (timeout <= 0) timeout = 1;
 switch(control_readfile(&rcpthosts,"control/rcpthosts",0))
  {
   case -1: die();
   case 1:
     rhok = 1;
     if (!constmap_init(&maprcpthosts,rcpthosts.s,rcpthosts.len,0)) die();
  }
}

void main()
{
 static stralloc cmd = {0};
 int match;

 signal_catchalarm(sigalrm);
 signal_init();

 if (chdir(CONF_HOME) == -1) die();
 getcontrols();
 getenvs();

 if (ipme_init() != 1) die();

 smtp_greet("220 ");
 out(" ESMTP\r\n");

 for (;;)
  {
   /* XXX: recipient can contain quoted lf. aargh. */
   if (getline2(&ssin,&cmd,&match,'\n') == -1) die();
   if (!match) die();
   if (cmd.len == 0) die();
   if (cmd.s[--cmd.len] != '\n') die();
   if ((cmd.len > 0) && (cmd.s[cmd.len - 1] == '\r')) --cmd.len;
   cmd.s[cmd.len++] = 0;
   doit(cmd.s);
  }
}
