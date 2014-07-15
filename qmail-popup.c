#include "commands.h"
#include "fd.h"
#include "sig.h"
#include "stralloc.h"
#include "substdio.h"
#include "alloc.h"
#include "wait.h"
#include "str.h"
#include "byte.h"
#include "now.h"
#include "fmt.h"
#include "exit.h"
#include "readwrite.h"
#include "timeoutread.h"
#include "timeoutwrite.h"

void die() { _exit(1); }

int saferead(fd,buf,len) int fd; char *buf; int len;
{
  int r;
  r = timeoutread(1200,fd,buf,len);
  if (r <= 0) die();
  return r;
}

int safewrite(fd,buf,len) int fd; char *buf; int len;
{
  int r;
  r = timeoutwrite(1200,fd,buf,len);
  if (r <= 0) die();
  return r;
}

char ssoutbuf[128];
substdio ssout = SUBSTDIO_FDBUF(safewrite,1,ssoutbuf,sizeof ssoutbuf);

char ssinbuf[128];
substdio ssin = SUBSTDIO_FDBUF(saferead,0,ssinbuf,sizeof ssinbuf);

void putstr(s) char *s;
{
  substdio_puts(&ssout,s);
}
void flush()
{
  substdio_flush(&ssout);
}
void err(s) char *s;
{
  putstr("-ERR ");
  putstr(s);
  putstr("\r\n");
  flush();
}

void die_usage() { err("usage: popup hostname subprogram"); die(); }
void die_nomem() { err("out of memory"); die(); }
void die_pipe() { err("unable to open pipe"); die(); }
void die_write() { err("unable to write pipe"); die(); }
void die_fork() { err("unable to fork"); die(); }
void die_childcrashed() { err("aack, child crashed"); }
void die_badauth() { err("authorization failed"); }

/* checkpassword error exit codes:
 * 1 = error in server configuration
 * 2 = unable to contact authorization server
 * 25= user record incorrect
 * 3 = authorization failed
 * 4 = account disabled
 * 5 = mailhost is unreachable
 * 6 = mailbox is corrupted
 * 61= mailbox creation failed
 * 62= the users mailbox does not exist
 * 7 = unable to start subprogram
 * 8 = out of memory
 */

void die_1() { err("error in server configuration"); die(); }
void die_2() { err("unable to contact authorization server"); die(); }
void die_25() { err("user record incorrect"); die(); }
void die_3() { err("authorization failed"); die(); }
void die_4() { err("account disabled"); die(); }
void die_5() { err("mailhost is unreachable"); die(); }
void die_6() { err("mailbox is corrupted"); die(); }
void die_61() { err("mailbox could not be created"); die(); }
void die_62() { err("the users mailbox does not exist"); die(); }
void die_7() { err("unable to start subprogram"); die(); }
void die_unknown() { err("temporary error"); die(); }

void err_syntax() { err("syntax error"); }
void err_wantuser() { err("USER first"); }
void err_authoriz() { err("authorization first"); }

void okay() { putstr("+OK \r\n"); flush(); }
void pop3_quit() { okay(); die(); }


char unique[FMT_ULONG + FMT_ULONG + 3];
char *hostname;
stralloc username = {0};
int seenuser = 0;
char **childargs;
substdio ssup;
char upbuf[128];


void doanddie(user,userlen,pass)
char *user;
unsigned int userlen; /* including 0 byte */
char *pass;
{
  int child;
  int wstat;
  int pi[2];
 
#ifndef DEBUG
  if (fd_copy(2,1) == -1) die_pipe();
#endif
  close(3);
  if (pipe(pi) == -1) die_pipe();
  if (pi[0] != 3) die_pipe();
  switch(child = fork()) {
    case -1:
      die_fork();
    case 0:
      close(pi[1]);
      sig_pipedefault();
      execvp(*childargs,childargs);
      _exit(1);
  }
  close(pi[0]);
  substdio_fdbuf(&ssup,write,pi[1],upbuf,sizeof upbuf);
  if (substdio_put(&ssup,user,userlen) == -1) die_write();
  if (substdio_put(&ssup,pass,str_len(pass) + 1) == -1) die_write();
  if (substdio_puts(&ssup,"<") == -1) die_write();
  if (substdio_puts(&ssup,unique) == -1) die_write();
  if (substdio_puts(&ssup,hostname) == -1) die_write();
  if (substdio_put(&ssup,">",2) == -1) die_write();
  if (substdio_flush(&ssup) == -1) die_write();
  close(pi[1]);
  byte_zero(pass,str_len(pass));
  byte_zero(upbuf,sizeof upbuf);
  if (wait_pid(&wstat,child) == -1) die();
  if (wait_crashed(wstat)) die_childcrashed();
  switch (wait_exitcode(wstat)) {
    case 0: die();
    case 1: die_1();
    case 2: die_2();
    case 25: die_25();
    case 3: die_3();
    case 4: die_4();
    case 5: die_5();
    case 6: die_6();
    case 61: die_61();
    case 62: die_62();
    case 7: die_7();
    case 8: die_nomem();
    default: die_unknown();
  }
  die();
}
void pop3_greet()
{
  char *s;
  s = unique;
  s += fmt_uint(s,getpid());
  *s++ = '.';
  s += fmt_ulong(s,(unsigned long) now());
  *s++ = '@';
  *s++ = 0;
  putstr("+OK <");
  putstr(unique);
  putstr(hostname);
  putstr(">\r\n");
  flush();
}
void pop3_user(arg) char *arg;
{
  if (!*arg) { err_syntax(); return; }
  okay();
  seenuser = 1;
  if (!stralloc_copys(&username,arg)) die_nomem(); 
  if (!stralloc_0(&username)) die_nomem(); 
}
void pop3_pass(arg) char *arg;
{
  if (!seenuser) { err_wantuser(); return; }
  if (!*arg) { err_syntax(); return; }
  doanddie(username.s,username.len,arg);
}
void pop3_apop(arg) char *arg;
{
  char *space;
  space = arg + str_chr(arg,' ');
  if (!*space) { err_syntax(); return; }
  *space++ = 0;
  doanddie(arg,space - arg,space);
}

struct commands pop3commands[] = {
  { "user", pop3_user, 0 }
, { "pass", pop3_pass, 0 }
, { "apop", pop3_apop, 0 }
, { "quit", pop3_quit, 0 }
, { "noop", okay, 0 }
, { 0, err_authoriz, 0 }
} ;

void main(argc,argv)
int argc;
char **argv;
{
  sig_alarmcatch(die);
  sig_pipeignore();
 
  hostname = argv[1];
  if (!hostname) die_usage();
  childargs = argv + 2;
  if (!*childargs) die_usage();
 
  pop3_greet();
  commands(&ssin,pop3commands);
  die();
}
