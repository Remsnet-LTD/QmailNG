#include "stralloc.h"
#include "substdio.h"
#include "qqtalk.h"
#include "now.h"
#include "str.h"
#include "fmt.h"
#include "env.h"
#include "signal.h"
#include "conf-home.h"
#include "now.h"
#include "datetime.h"
#include "date822fmt.h"
#include "readwrite.h"
#include "control.h"
#include "constmap.h"

static char subfd_outbuf[128];
static struct substdio out = SUBSTDIO_FDBUF(write,1,subfd_outbuf,128);
static struct substdio *subfdout = &out;

static int subfd_read(fd,buf,len) int fd; char *buf; int len;
{
 if (substdio_flush(subfdout) == -1) return -1;
 return read(fd,buf,len);
}

static char subfd_inbuf[1024];
static struct substdio in = SUBSTDIO_FDBUF(subfd_read,0,subfd_inbuf,1024);
static struct substdio *subfdin = &in;

struct qqtalk qqt;

void dropped() { _exit(0); }
void badproto() { _exit(1); }
void resources() { _exit(111); }
void sigalrm() { _exit(111); }

unsigned long getlen()
{
 unsigned long len;
 char ch;

 len = 0;
 for (;;)
  {
   if (substdio_get(subfdin,&ch,1) < 1) dropped();
   if (ch == ':') return len;
   if (len > 200000000) resources();
   len = 10 * len + (ch - '0');
  }
}

void getcomma()
{
 char ch;

 if (substdio_get(subfdin,&ch,1) < 1) dropped();
 if (ch != ',') badproto();
}

static int issafe(ch) char ch;
{
 if (ch == '.') return 1;
 if (ch == '@') return 1;
 if (ch == '%') return 1;
 if (ch == '+') return 1;
 if (ch == '/') return 1;
 if (ch == '=') return 1;
 if (ch == ':') return 1;
 if (ch == '-') return 1;
 if ((ch >= 'a') && (ch <= 'z')) return 1;
 if ((ch >= 'A') && (ch <= 'Z')) return 1;
 if ((ch >= '0') && (ch <= '9')) return 1;
 return 0;
}

void safeput(s) char *s;
{
 char ch;
 while (ch = *s++)
  {
   if (!issafe(ch)) ch = '?';
   qqtalk_put(&qqt,&ch,1);
  }
}

struct datetime dt;
char buf[1000];
char buf2[100];

char *remotehost;
char *remoteinfo;
char *remoteip;
char *local;

stralloc failure = {0};

int flagrcpthosts;
stralloc rcpthosts = {0};
struct constmap maprcpthosts;
char *relayclient;
int relayclientlen;

int addrallowed(buf,len) char *buf; int len;
{
 int j;
 if (!flagrcpthosts) return 1;
 j = byte_rchr(buf,len,'@');
 if (j >= len) return 1;
 if (constmap(&maprcpthosts,buf + j + 1,len - j - 1)) return 1;
 for (;j < len;++j)
   if (buf[j] == '.')
     if (constmap(&maprcpthosts,buf + j,len - j)) return 1;
 return 0;
}

main()
{
 char ch;
 int i;
 unsigned long biglen;
 unsigned long len;
 int flagdos;
 int flagsenderok;
 unsigned long qp;
 char *result;

 signal_init();
 signal_catchalarm(sigalrm);
 alarm(3600);

 if (chdir(CONF_HOME) == -1) resources();

 if (control_init() == -1) resources();
 flagrcpthosts = control_readfile(&rcpthosts,"control/rcpthosts",0);
 if (flagrcpthosts == -1) resources();
 if (flagrcpthosts)
   if (!constmap_init(&maprcpthosts,rcpthosts.s,rcpthosts.len,0)) resources();
 relayclient = env_get("RELAYCLIENT");
 relayclientlen = relayclient ? str_len(relayclient) : 0;

 remotehost = env_get("TCPREMOTEHOST");
 if (!remotehost) remotehost = "unknown";
 remoteinfo = env_get("TCPREMOTEINFO");
 remoteip = env_get("TCPREMOTEIP");
 if (!remoteip) remoteip = "unknown";
 local = env_get("TCPLOCALHOST");
 if (!local) local = env_get("TCPLOCALIP");
 if (!local) local = "unknown";

 for (;;)
  {
   if (!stralloc_copys(&failure,"")) resources();
   flagsenderok = 1;

   len = getlen();
   if (len == 0) badproto();

   if (qqtalk_open(&qqt,0) == -1) resources();
   qp = qqtalk_qp(&qqt);

   if (substdio_get(subfdin,&ch,1) < 1) dropped();
   --len;

   if (ch == 10) flagdos = 0;
   else if (ch == 13) flagdos = 1;
   else badproto();

   qqtalk_puts(&qqt,"Received: from ");
   safeput(remotehost);
   qqtalk_puts(&qqt," (");
   if (remoteinfo)
    {
     safeput(remoteinfo);
     qqtalk_puts(&qqt,"@");
    }
   safeput(remoteip);
   qqtalk_puts(&qqt,")\n  by ");
   safeput(local);
   qqtalk_puts(&qqt," with QMTP; ");
   datetime_tai(&dt,now());
   qqtalk_put(&qqt,buf2,date822fmt(buf2,&dt));

   /* XXX: check for loops? only if len is big? */

   if (flagdos)
     while (len > 0)
      {
       if (substdio_get(subfdin,&ch,1) < 1) dropped();
       --len;
       while ((ch == 13) && len)
        {
         if (substdio_get(subfdin,&ch,1) < 1) dropped();
         --len;
         if (ch == 10) break;
         qqtalk_put(&qqt,"\015",1);
        }
       qqtalk_put(&qqt,&ch,1);
      }
   else
     while (len > 0) /* XXX: could speed this up, obviously */
      {
       if (substdio_get(subfdin,&ch,1) < 1) dropped();
       --len;
       qqtalk_put(&qqt,&ch,1);
      }
   getcomma();

   len = getlen();

   if (len >= 1000)
    {
     buf[0] = 0;
     flagsenderok = 0;
     for (i = 0;i < len;++i)
       if (substdio_get(subfdin,&ch,1) < 1) dropped();
    }
   else
    {
     for (i = 0;i < len;++i)
      {
       if (substdio_get(subfdin,buf + i,1) < 1) dropped();
       if (!buf[i]) flagsenderok = 0;
      }
     buf[len] = 0;
    }
   getcomma();

   qqtalk_from(&qqt,buf);
   if (!flagsenderok) qqtalk_fail(&qqt);

   biglen = getlen();
   while (biglen > 0)
    {
     if (!stralloc_append(&failure,"")) resources();

     len = 0;
     for (;;)
      {
       if (!biglen) badproto();
       if (substdio_get(subfdin,&ch,1) < 1) dropped();
       --biglen;
       if (ch == ':') break;
       if (len > 200000000) resources();
       len = 10 * len + (ch - '0');
      }
     if (len >= biglen) badproto();
     if (len + relayclientlen >= 1000)
      {
       failure.s[failure.len - 1] = 'L';
       for (i = 0;i < len;++i)
         if (substdio_get(subfdin,&ch,1) < 1) dropped();
      }
     else
      {
       for (i = 0;i < len;++i)
        {
         if (substdio_get(subfdin,buf + i,1) < 1) dropped();
         if (!buf[i]) failure.s[failure.len - 1] = 'N';
        }
       buf[len] = 0;

       if (relayclient)
	 str_copy(buf + len,relayclient);
       else
	 if (!addrallowed(buf,len)) failure.s[failure.len - 1] = 'D';

       if (!failure.s[failure.len - 1])
	 qqtalk_to(&qqt,buf);
      }
     getcomma();
     biglen -= (len + 1);
    }
   getcomma();

   switch(qqtalk_close(&qqt))
    {
     case 0: result = 0; break;
     case QQT_WAITPID: result = "Zqq waitpid surprise (#4.3.0)"; break;
     case QQT_CRASHED: result = "Zqq crashed (#4.3.0)"; break;
     case QQT_USAGE: result = "Zqq usage surprise (#4.3.0)"; break;
     case QQT_SYS: result = "Zqq system error (#4.3.0)"; break;
     case QQT_READ: result = "Zqq read error (#4.3.0)"; break;
     case QQT_WRITE: result = "Zqq write error or disk full (#4.3.0)"; break;
     case QQT_NOMEM: result = "Zqq out of memory (#4.3.0)"; break;
     case QQT_EXECSOFT: result = "Zcould not exec qq (#4.3.0)"; break;
     case QQT_EXECHARD: result = "Dcould not exec qq (#5.3.5)"; break;
     case QQT_TIMEOUT: result = "Zqq timeout (#4.3.0)"; break;
     case QQT_TOOLONG: result = "Dqq toolong surprise (#5.1.3)"; break;
     default: result = "Zqq internal bug (#4.3.0)"; break;
    }

   if (!flagsenderok) result = "Dunacceptable sender (#5.1.7)";

   if (result)
     len = str_len(result);
   else
    {
     /* success! */
     len = 0;
     len += fmt_str(buf2 + len,"Kok ");
     len += fmt_ulong(buf2 + len,(unsigned long) now());
     len += fmt_str(buf2 + len," qp ");
     len += fmt_ulong(buf2 + len,qp);
     buf2[len] = 0;
     result = buf2;
    }
     
   len = fmt_ulong(buf,len);
   buf[len++] = ':';
   len += fmt_str(buf + len,result);
   buf[len++] = ',';

   for (i = 0;i < failure.len;++i)
     switch(failure.s[i])
      {
       case 0:
         if (substdio_put(subfdout,buf,len) == -1)
           dropped();
	 break;
       case 'D':
         if (substdio_puts(subfdout,"66:Dsorry, that domain isn't in my list of allowed rcpthosts (#5.7.1),") == -1)
	   dropped();
	 break;
       default:
         if (substdio_puts(subfdout,"46:Dsorry, I can't handle that recipient (#5.1.3),") == -1)
	   dropped();
	 break;
      }

   /* subfdout will be flushed when we read from the network again */
  }
}
