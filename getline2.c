#include "substdio.h"
#include "byte.h"
#include "stralloc.h"
#include "alloc.h"
#include "getline.h"

int getline2(ss,sa,match,sep)
register substdio *ss;
register stralloc *sa;
int *match;
int sep;
{
 register int i;
 char *cont;
 int clen;

 if (getline(ss,sa,&cont,&clen,sep) == -1)
   return -1;
 if (!clen)
  {
   *match = 0;
   return 0;
  }
 if (!stralloc_readyplus(sa,clen))
   return -1;
 i = sa->len;
 byte_copy(sa->s + i,clen,cont);
 sa->len = i + clen;
 *match = 1;
 return 0;
}
