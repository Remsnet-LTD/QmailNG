#include "signal.h"
#include "readwrite.h"
#include "substdio.h"
#include "stralloc.h"
#include "subfd.h"
#include "getline.h"
#include "alloc.h"
#include "str.h"
#include "env.h"
#include "hfield.h"
#include "case.h"
#include "token822.h"
#include "error.h"
#include "gen_alloc.h"
#include "gen_allocdefs.h"
#include "headerbody.h"
#include "exit.h"
#include "open.h"
#include "lock.h"
#include "qqtalk.h"

#define ADDRLIMIT 100

void die() { _exit(1); }
void die_temp() { _exit(111); }
void die_nomem() {
 substdio_putsflush(subfderr,"qlist: fatal: out of memory\n"); die_temp(); }
void die_fork() {
 substdio_putsflush(subfderr,"qlist: fatal: unable to fork\n"); die_temp(); }
void die_nolock() {
 substdio_putsflush(subfderr,"qlist: fatal: unable to open lock file\n"); die_temp(); }
void die_boing() {
 substdio_putsflush(subfderr,"qlist: fatal: I don't reply to bounces\n"); die(); }
void die_badaddr() {
 substdio_putsflush(subfderr,"qlist: fatal: sorry, I'm not allowed to use that address\n"); die(); }
void die_qqperm() {
 substdio_putsflush(subfderr,"qlist: fatal: permanent qmail-queue error\n"); die(); }
void die_qqtemp() {
 substdio_putsflush(subfderr,"qlist: fatal: temporary qmail-queue error\n"); die_temp(); }
void die_usage() {
 substdio_putsflush(subfderr,
 "qlist: usage: qlist user-list@host user-list-request@host .qmail-list .qmail-list-request .qtemp-list owner [moreinfo]\n"); die(); }
void die_read() {
 if (errno == error_nomem) die_nomem();
 substdio_putsflush(subfderr,"qlist: fatal: read error\n"); die_temp(); }
void doordie(sa,r) stralloc *sa; int r; {
 if (r == 1) return; if (r == -1) die_nomem();
 substdio_putsflush(subfderr,"qlist: fatal: unable to parse this: ");
 substdio_putflush(subfderr,sa->s,sa->len); die(); }

int subjectaction = 0;
int numcommands;

int fdlock;

struct qqtalk qqt;

char *target;
char *listathost;
char *requestathost;
char *qmaillist;
char *qmailrequest;
char *qtemplist;
char *owner;
char *moreinfo;

char *dtline;
char *returnpath;
stralloc safrom = {0};
stralloc sart = {0};

int rwfrom(addr) token822_alloc *addr; { token822_reverse(addr);
 if (token822_unquote(&safrom,addr) != 1) die_nomem();
 token822_reverse(addr); return 1; }
int rwrt(addr) token822_alloc *addr; { token822_reverse(addr);
 if (token822_unquote(&sart,addr) != 1) die_nomem();
 token822_reverse(addr); return 1; }

GEN_ALLOC_typedef(saa,stralloc,sa,len,a)
GEN_ALLOC_readyplus(saa,stralloc,sa,len,a,i,n,x,10,saa_readyplus)
static stralloc sauninit = {0}; saa savedh = {0};
void savedh_append(h) stralloc *h; {
 if (!saa_readyplus(&savedh,1)) die_nomem(); savedh.sa[savedh.len] = sauninit;
 if (!stralloc_copy(savedh.sa + savedh.len,h)) die_nomem(); ++savedh.len; }
void savedh_print() { int i; for (i = 0;i < savedh.len;++i)
 qqtalk_put(&qqt,savedh.sa[i].s,savedh.sa[i].len); }

void finishheader()
{
 int i;

 if (sart.s)
  { if (!stralloc_0(&sart)) die_nomem(); target = sart.s; }
 else if (safrom.s)
  { if (!stralloc_0(&safrom)) die_nomem(); target = safrom.s; }
 else
   target = returnpath;

 for (i = 0;target[i];++i)
   if (target[i] == '\n')
     die_badaddr();
 if (i > ADDRLIMIT) die_badaddr();
 if (!str_diff(target,"")) die_boing();
 if (!str_diff(target,"#@[]")) die_boing();

 if (qqtalk_open(&qqt,1) == -1) die_fork();

 qqtalk_puts(&qqt,dtline);
 savedh_print();

 qqtalk_puts(&qqt,"\n***** Text inserted by ");
 qqtalk_puts(&qqt,requestathost);
 qqtalk_puts(&qqt,"\n\
*\n\
* Hi! This is the qlist program. I'm handling subscriptions for the\n\
* ");
 qqtalk_puts(&qqt,listathost);
 qqtalk_puts(&qqt," mailing list.\n\
*\n");
 if (moreinfo)
  {
   qqtalk_puts(&qqt,"* ");
   qqtalk_puts(&qqt,moreinfo);
   qqtalk_puts(&qqt,"\n*\n");
  }
 qqtalk_puts(&qqt,"* My human owner is ");
 qqtalk_puts(&qqt,owner);
 qqtalk_puts(&qqt,".\n\
*\n\
* To the recipient: This message was sent to me on your behalf. (Your\n\
* address was listed in the Reply-To or From field.) For security,\n\
* I'm forwarding this message to you, along with my notes.\n\
*\n\
* Anyway, to subscribe, send me an empty message. To unsubscribe, send me\n\
* a message with the word UNSUBSCRIBE at the beginning of a line. Remember,\n\
* my address is ");
 qqtalk_puts(&qqt,requestathost);
 qqtalk_puts(&qqt,".\n\
*\n\
* Now I'll look for requests inside this message...\n\
*\n\
*****\n");
}

substdio subin; char subinbuf[SUBSTDIO_INSIZE];
substdio subout; char suboutbuf[SUBSTDIO_OUTSIZE];
stralloc subline = {0};
void subscribe(flagadd)
int flagadd;
{
 int fdin;
 int fdout;
 int match;
 int flagwasthere;

 ++numcommands;

 if (lock_ex(fdlock) == -1) goto bad;
 fdin = open_read(qmaillist);
 if (fdin == -1) goto badlock;
 fdout = open_trunc(qtemplist);
 if (fdout == -1) goto badinlock;
 if (chmod(qtemplist,0700) == -1) goto badoutinlock;

 flagwasthere = 0;

 substdio_fdbuf(&subin,read,fdin,subinbuf,sizeof(subinbuf));
 substdio_fdbuf(&subout,write,fdout,suboutbuf,sizeof(suboutbuf));
 for (;;)
  {
   if (getline2(&subin,&subline,&match,'\n') == -1) goto badoutinlock;
   if (!match) break; /* goodbye partial lines */
   if (subline.len == str_len(target) + 2)
     if (!str_diffn(subline.s + 1,target,subline.len - 2))
       if (subline.s[0] == '&')
        {
         flagwasthere = 1;
         if (!flagadd)
           continue;
        }
   if (substdio_put(&subout,subline.s,subline.len) == -1) goto badoutinlock;
  }

 if (flagadd && !flagwasthere)
  {
   if (substdio_puts(&subout,"&") == -1) goto badoutinlock;
   if (substdio_puts(&subout,target) == -1) goto badoutinlock;
   if (substdio_puts(&subout,"\n") == -1) goto badoutinlock;
  }
 if (substdio_flush(&subout) == -1) goto badoutinlock;

 close(fdout);
 close(fdin);
 if (rename(qtemplist,qmaillist) == -1) goto badlock;
 if (chmod(qmaillist,0500) == -1) goto badlock;

 lock_un(fdlock);

 qqtalk_puts(&qqt,"***** Text inserted by ");
 qqtalk_puts(&qqt,requestathost);
 qqtalk_puts(&qqt,"\n*\n* ");
 if (flagadd)
   if (flagwasthere)
    {
     qqtalk_puts(&qqt,"Acknowledgment: ");
     qqtalk_puts(&qqt,target);
     qqtalk_puts(&qqt," was already a subscriber.\n");
    }
   else
    {
     qqtalk_puts(&qqt,"Acknowledgment: ");
     qqtalk_puts(&qqt,target);
     qqtalk_puts(&qqt," is now a subscriber.\n");
    }
 else
   if (flagwasthere)
    {
     qqtalk_puts(&qqt,"Acknowledgment: ");
     qqtalk_puts(&qqt,target);
     qqtalk_puts(&qqt," is no longer a subscriber.\n");
    }
   else
    {
     qqtalk_puts(&qqt,"Hmmm, I don't see ");
     qqtalk_puts(&qqt,target);
     qqtalk_puts(&qqt," on the subscription list.\n* I'll let my owner know.\n");
    }
 qqtalk_puts(&qqt,"*\n*****\n");
 return;

badoutinlock: close(fdout);
badinlock: close(fdin);
badlock: lock_un(fdlock);
bad:
 qqtalk_puts(&qqt,"***** Text inserted by ");
 qqtalk_puts(&qqt,requestathost);
 qqtalk_puts(&qqt,"\n*\n\
* Oh no! Trouble making the new list. I'll let my owner know.\n\
*\n\
*****\n");
}

void dobody(h) stralloc *h;
{
 qqtalk_put(&qqt,h->s,h->len);
 if (!case_diffb(h->s,"subs",4)) subscribe(1);
 if (!case_diffb(h->s,"unsu",4)) subscribe(0);
}

stralloc hfbuf = {0};
token822_alloc hfin = {0};
token822_alloc hfrewrite = {0};
token822_alloc hfaddr = {0};

void doheaderfield(h)
stralloc *h;
{
 char *x;
 switch(hfield_known(h->s,h->len))
  {
   case H_CONTENTLENGTH: /* SVR4 silliness */
   case H_CONTENTTYPE:
   case H_CONTENTTRANSFERENCODING: /* A-bombs 4.2.1.5.2 is idiotic */
     return;
   case H_FROM:
     doordie(h,token822_parse(&hfin,h,&hfbuf));
     doordie(h,token822_addrlist(&hfrewrite,&hfaddr,&hfin,rwfrom));
     break;
   case H_REPLYTO:
     doordie(h,token822_parse(&hfin,h,&hfbuf));
     doordie(h,token822_addrlist(&hfrewrite,&hfaddr,&hfin,rwrt));
     break;
   case H_SUBJECT:
     x = h->s + hfield_skipname(h->s,h->len);
     if (!case_diffb(x,"subs",4)) subjectaction = 1;
     if (!case_diffb(x,"unsu",4)) subjectaction = 2;
     break;
  }
 savedh_append(h);
}

void main(argc,argv)
int argc;
char **argv;
{
 signal_init();

 if (!(listathost = argv[1])) die_usage();
 if (!(requestathost = argv[2])) die_usage();
 if (!(qmaillist = argv[3])) die_usage();
 if (!(qmailrequest = argv[4])) die_usage();
 if (!(qtemplist = argv[5])) die_usage();
 if (!(owner = argv[6])) die_usage();
 moreinfo = argv[7];
 if (!(returnpath = env_get("NEWSENDER"))) die_usage();
 if (!(dtline = env_get("DTLINE"))) die_usage();

 fdlock = open_append(qmailrequest);
 if (fdlock == -1) die_nolock();

 numcommands = 0;
 if (headerbody(subfdin,doheaderfield,finishheader,dobody) == -1) die_read();
 if (!numcommands)
  {
   qqtalk_puts(&qqt,"***** Text inserted by ");
   qqtalk_puts(&qqt,requestathost);
   qqtalk_puts(&qqt,"\n*\n* ");
   if (subjectaction)
    {
     qqtalk_puts(&qqt,"\
Hmmm, no commands? Let me check the Subject line...\n*\n*****\n");
     subscribe(subjectaction == 1);
    }
   else
    {
     qqtalk_puts(&qqt,"\
I didn't see any commands. I presume this is a subscription request.\n\
*\n*****\n");
     subscribe(1);
    }
  }

 qqtalk_from(&qqt,returnpath);
 qqtalk_to(&qqt,owner);
 qqtalk_to(&qqt,target);

 switch(qqtalk_close(&qqt))
  {
   case 0: _exit(0);
   case QQT_TOOLONG: die_qqperm();
   default: die_qqtemp();
  }
}
