#include "fmt.h"
#include "qqtalk.h"
#include "now.h"
#include "datetime.h"
#include "date822fmt.h"
#include "received.h"

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

void safeput(qqt,s)
struct qqtalk *qqt;
char *s;
{
  char ch;
  while (ch = *s++) {
    if (!issafe(ch)) ch = '?';
    qqtalk_put(qqt,&ch,1);
  }
}

static char buf[DATE822FMT];

/* "Received: from relay1.uu.net (HELO uunet.uu.net) (7@192.48.96.5)\n" */
/* "  by silverton.berkeley.edu with SMTP; 26 Sep 1995 04:46:54 -0000\n" */

void received(qqt,protocol,local,remoteip,remotehost,remoteinfo,helo)
struct qqtalk *qqt;
char *protocol;
char *local;
char *remoteip;
char *remotehost;
char *remoteinfo;
char *helo;
{
  struct datetime dt;

  qqtalk_puts(qqt,"Received: from ");
  safeput(qqt,remotehost);
  if (helo) {
    qqtalk_puts(qqt," (HELO ");
    safeput(qqt,helo);
    qqtalk_puts(qqt,")");
  }
  qqtalk_puts(qqt," (");
  if (remoteinfo) {
    safeput(qqt,remoteinfo);
    qqtalk_puts(qqt,"@");
  }
  safeput(qqt,remoteip);
  qqtalk_puts(qqt,")\n  by ");
  safeput(qqt,local);
  qqtalk_puts(qqt," with ");
  qqtalk_puts(qqt,protocol);
  qqtalk_puts(qqt,"; ");
  datetime_tai(&dt,now());
  qqtalk_put(qqt,buf,date822fmt(buf,&dt));
}
