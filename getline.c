#include "substdio.h"
#include "alloc.h"
#include "stralloc.h"
#include "byte.h"
#include "getline.h"

int getline(ss,sa,cont,clen,sep)
register substdio *ss;
register stralloc *sa;
char **cont;
int *clen;
int sep;
{
 register char *x;
 register int i;
 int n;

 if (!stralloc_ready(sa,0))
   return -1;
 sa->len = 0;

 for (;;)
  {
   x = substdio_PEEK(ss,&n);
   if (n)
    {
     i = byte_chr(x,n,sep);
     if (i < n) { substdio_SEEK(ss,*clen = i + 1); *cont = x; return 0; }
     if (!stralloc_readyplus(sa,n))
       return -1;
     i = sa->len;
     sa->len = i + substdio_get(ss,sa->s + i,n);
    }
   n = substdio_feed(ss);
   if (n < 0) return -1;
   if (n == 0) { *clen = 0; return 0; }
  }
}
