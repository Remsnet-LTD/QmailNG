#include "fmt.h"
#include "datetime.h"
#include "alloc.h"
#include "date822fmt.h"
#include "newfield.h"

char *newfield_cc = "Cc: recipient list not shown: ;\n";
/* "Date: 26 Sep 1995 04:46:53 -0000\n\0" */
char *newfield_date = 0; static int newfield_datealloc;
/* "Message-ID: <19950926044653.12345.qmail@silverton.berkeley.edu>\n\0" */
char *newfield_msgid = 0;
/* "Received: from relay1.uu.net (HELO uunet.uu.net) (7@192.48.96.5)\n" */
/* "  by silverton.berkeley.edu with SMTP; 26 Sep 1995 04:46:54 -0000\n\0" */
char *newfield_rec = 0;

static unsigned int datefmt(s,when)
char *s;
datetime_sec when;
{
 unsigned int i;
 unsigned int len;
 struct datetime dt;
 datetime_tai(&dt,when);
 len = 0;
 i = fmt_str(s,"Date: "); len += i; if (s) s += i;
 i = date822fmt(s,&dt); len += i; if (s) s += i;
 ++len; if (s) *s = 0;
 return len;
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

static unsigned int fmtsafe(s,t)
char *s;
char *t;
{
 unsigned int len;
 unsigned int j;
 len = fmt_str(s,t);
 if (s)
   for (j = 0;j < len;++j)
     if (!issafe(s[j]))
       s[j] = '?';
 return len;
}

static unsigned int recfmt(s,helo,real,rinfo,dddd,myname)
char *s;
char *helo;
char *real;
char *rinfo;
char *dddd;
char *myname;
{
 unsigned int i;
 unsigned int len;
 len = 0;
 i = fmt_str(s,"Received: from "); len += i; if (s) s += i;
 if (real)
  {
   i = fmtsafe(s,real); len += i; if (s) s += i;
   i = fmt_str(s," (HELO "); len += i; if (s) s += i;
   i = fmtsafe(s,helo); len += i; if (s) s += i;
   i = fmt_str(s,")"); len += i; if (s) s += i;
  }
 else
  {
   i = fmtsafe(s,helo); len += i; if (s) s += i;
  }
 i = fmt_str(s," ("); len += i; if (s) s += i;
 if (rinfo)
  {
   i = fmtsafe(s,rinfo); len += i; if (s) s += i;
   i = fmt_str(s,"@"); len += i; if (s) s += i;
  }
 i = fmtsafe(s,dddd); len += i; if (s) s += i;
 i = fmt_str(s,")\n  by "); len += i; if (s) s += i;
 i = fmtsafe(s,myname); len += i; if (s) s += i;
 i = fmt_str(s," with SMTP; "); len += i; if (s) s += i;
 i = fmt_str(s,newfield_date + 6); len += i; if (s) s += i;
 ++len; if (s) *s = 0;
 return len;
}

static unsigned int msgidfmt(s,idhost,idhostlen,when)
char *s;
char *idhost;
int idhostlen;
datetime_sec when;
{
 unsigned int i;
 unsigned int len;
 struct datetime dt;
 datetime_tai(&dt,when);
 len = 0;
 i = fmt_str(s,"Message-ID: <"); len += i; if (s) s += i;
 i = fmt_uint(s,dt.year + 1900); len += i; if (s) s += i;
 i = fmt_uint0(s,dt.mon + 1,2); len += i; if (s) s += i;
 i = fmt_uint0(s,dt.mday,2); len += i; if (s) s += i;
 i = fmt_uint0(s,dt.hour,2); len += i; if (s) s += i;
 i = fmt_uint0(s,dt.min,2); len += i; if (s) s += i;
 i = fmt_uint0(s,dt.sec,2); len += i; if (s) s += i;
 i = fmt_str(s,"."); len += i; if (s) s += i;
 i = fmt_uint(s,getpid()); len += i; if (s) s += i;
 i = fmt_str(s,".qmail@"); len += i; if (s) s += i;
 i = fmt_strn(s,idhost,idhostlen); len += i; if (s) s += i;
 i = fmt_str(s,">\n"); len += i; if (s) s += i;
 ++len; if (s) *s = 0;
 return len;
}

int newfield_datemake(when)
datetime_sec when;
{
 unsigned int len;
 len = datefmt((char *) 0,when);
 if (!newfield_date)
  {
   newfield_date = alloc(len);
   newfield_datealloc = len;
   if (!newfield_date) return 0;
  }
 if (newfield_datealloc < len)
  {
   if (!alloc_re(&newfield_date,newfield_datealloc,len)) return 0;
   newfield_datealloc = len;
  }
 datefmt(newfield_date,when);
 return 1;
}

int newfield_msgidmake(idhost,idhostlen,when)
char *idhost;
int idhostlen;
datetime_sec when;
{
 if (newfield_msgid) return 1;
 newfield_msgid = alloc(msgidfmt((char *) 0,idhost,idhostlen,when));
 if (!newfield_msgid) return 0;
 msgidfmt(newfield_msgid,idhost,idhostlen,when);
 return 1;
}

int newfield_recmake(helo,real,rinfo,dddd,myname,when)
char *helo;
char *real;
char *rinfo;
char *dddd;
char *myname;
datetime_sec when;
{
 if (newfield_rec) return 1;
 if (!newfield_datemake(when)) return 0;
 newfield_rec = alloc(recfmt((char *) 0,helo,real,rinfo,dddd,myname));
 if (!newfield_rec) return 0;
 recfmt(newfield_rec,helo,real,rinfo,dddd,myname);
 return 1;
}

int newfield_ccmake()
{
 return 1;
}
