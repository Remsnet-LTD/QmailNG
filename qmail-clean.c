#include <sys/types.h>
#include <sys/stat.h>
#include "readwrite.h"
#include "signal.h"
#include "now.h"
#include "str.h"
#include "direntry.h"
#include "getline.h"
#include "stralloc.h"
#include "substdio.h"
#include "byte.h"
#include "scan.h"
#include "fmt.h"
#include "fmtqfn.h"
#include "error.h"
#include "exit.h"
#include "conf-home.h"

#define OSSIFIED 129600 /* see qmail-send.c */

stralloc line = {0};

void cleanuppid()
{
 DIR *dir;
 direntry *d;
 struct stat st;
 datetime_sec time;

 time = now();
 dir = opendir("pid");
 if (!dir) return;
 while (d = readdir(dir))
  {
   if (!str_diff(d->d_name,".")) continue;
   if (!str_diff(d->d_name,"..")) continue;
   if (!stralloc_copys(&line,"pid/")) continue;
   if (!stralloc_cats(&line,d->d_name)) continue;
   if (!stralloc_0(&line)) continue;
   if (stat(line.s,&st) == -1) continue;
   if (time < st.st_atime + OSSIFIED) continue;
   unlink(line.s);
  }
 closedir(dir);
}

char outbuf[2]; substdio ssout;
char inbuf[128]; substdio ssin;
char fnbuf[FMTQFN];

void respond(s) char *s; { if (substdio_putflush(&ssout,s,1) == -1) _exit(1); }

void main()
{
 int i;
 int match;
 int cleanuploop;
 unsigned long id;

 if (chdir(CONF_HOME) == -1) _exit(1);
 if (chdir("queue") == -1) _exit(1);

 signal_init();

 substdio_fdbuf(&ssout,write,1,outbuf,sizeof(outbuf));
 substdio_fdbuf(&ssin,read,0,inbuf,sizeof(inbuf));

 if (!stralloc_ready(&line,200)) _exit(1);

 cleanuploop = 0;

 for (;;)
  {
   if (cleanuploop) --cleanuploop; else { cleanuppid(); cleanuploop = 30; }
   if (getline2(&ssin,&line,&match,'\0') == -1) break;
   if (!match) break;
   if (line.len < 7) { respond("x"); continue; }
   if (line.len > 100) { respond("x"); continue; }
   if (line.s[line.len - 1]) { respond("x"); continue; } /* impossible */
   for (i = 5;i < line.len - 1;++i)
     if ((unsigned char) (line.s[i] - '0') > 9)
      { respond("x"); continue; }
   if (!scan_ulong(line.s + 5,&id)) { respond("x"); continue; }
   if (!byte_diff(line.s,5,"foop/"))
    {
#define U(prefix,flag) fmtqfn(fnbuf,prefix,id,flag); \
if (unlink(fnbuf) == -1) if (errno != error_noent) { respond("!"); continue; }
     U("intd/",0)
     U("mess/",1)
     respond("+");
    }
   else if (!byte_diff(line.s,4,"todo/"))
    {
     U("intd/",0)
     U("todo/",0)
     respond("+");
    }
   else
     respond("x");
  }
 _exit(0);
}
