#include "case.h"

void case_lowers(s)
char *s;
{
 unsigned char x;
 while (x = *s)
  {
   x -= 'A';
   if (x <= 'Z' - 'A')
     *s = x + 'a';
   ++s;
  }
}

void case_lowerb(s,len)
char *s;
unsigned int len;
{
 unsigned char x;
 while (len > 0)
  {
   --len;
   x = *s - 'A';
   if (x <= 'Z' - 'A')
     *s = x + 'a';
   ++s;
  }
}

int case_diffs(s,t)
register char *s;
register char *t;
{
 register unsigned char x;
 register unsigned char y;

 for (;;)
  {
   x = *s++ - 'A';
   if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
   y = *t++ - 'A';
   if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
   if (x != y) break;
   if (!x) break;
  }
 return ((int)(unsigned int) x) - ((int)(unsigned int) y);
}

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
