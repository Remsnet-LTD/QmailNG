#include "case.h"

int case_diffb(s,t,len)
register char *s;
register char *t;
unsigned int len;
{
 register unsigned char x;
 register unsigned char y;

 while (len > 0)
  {
   --len;
   x = *s++ - 'A';
   if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
   y = *t++ - 'A';
   if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
   if (x != y)
     return ((int)(unsigned int) x) - ((int)(unsigned int) y);
  }
 return 0;
}
