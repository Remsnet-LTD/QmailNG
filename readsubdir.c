#include "readsubdir.h"
#include "fmt.h"
#include "scan.h"
#include "str.h"
#include "conf-unusual.h"

void readsubdir_init(rs,name,pause)
readsubdir *rs;
char *name;
void (*pause)();
{
 rs->name = name;
 rs->pause = pause;
 rs->dir = 0;
 rs->pos = 0;
}

static char namepos[FMT_ULONG + 20/*XXX*/];

int readsubdir_next(rs,id)
readsubdir *rs;
unsigned long *id;
{
 direntry *d;
 unsigned int len;

 if (!rs->dir)
  {
   if (rs->pos >= QFN_SPLIT) return 0;
   len = 0;
   len += fmt_str(namepos + len,rs->name);
   namepos[len++] = '/';
   len += fmt_ulong(namepos + len,(unsigned long) rs->pos);
   namepos[len] = 0;
   while (!(rs->dir = opendir(namepos))) rs->pause(namepos);
   rs->pos++;
   return -1;
  }

 d = readdir(rs->dir);
 if (!d) { closedir(rs->dir); rs->dir = 0; return -1; }

 if (!str_diff(d->d_name,".")) return -1;
 if (!str_diff(d->d_name,"..")) return -1;
 len = scan_ulong(d->d_name,id);
 if (!len || d->d_name[len]) return -2;
 return 1;
}
