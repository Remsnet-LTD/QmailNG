#include "getline.h"
#include "stralloc.h"
#include "substdio.h"
#include "subfd.h"
#include "error.h"
#include "alloc.h"
#include "token822.h"

int callback(taaddr)
token822_alloc *taaddr;
{
 int i;
 if (!token822_readyplus(taaddr,2))
   return -1;
 i = taaddr->len;
 taaddr->len = i + 2;
 taaddr->t[i + 1].type = TOKEN822_ATOM;
 taaddr->t[i + 1].s = "{";
 taaddr->t[i + 1].slen = 1;
 while (i > 0)
  {
   taaddr->t[i] = taaddr->t[i - 1];
   --i;
  }
 taaddr->t[0].type = TOKEN822_ATOM;
 taaddr->t[0].s = "}";
 taaddr->t[0].slen = 1;
 return 1;
}

stralloc sa = {0};
stralloc buf = {0};
token822_alloc taout = {0};
token822_alloc taaddr = {0};
token822_alloc ta = {0};

void main()
{
 int match;

 while ((getline2(subfdin,&sa,&match,'\n') != -1) && match)
  {
   substdio_puts(subfdout,"original: ");
   substdio_putflush(subfdout,sa.s,sa.len);
   switch (token822_parse(&ta,&sa,&buf))
    {
     case -1:
       substdio_putsflush(subfdout,"out of memory\n");
       break;
     case 0:
       substdio_putsflush(subfdout,"illegal\n");
       break;
     default:
       if (token822_unparse(&sa,&ta,0) == -1)
         substdio_putsflush(subfdout,"out of memory\n");
       else
	{
         substdio_puts(subfdout,"respaced: ");
	 substdio_putflush(subfdout,sa.s,sa.len);
	 switch(token822_addrlist(&taout,&taaddr,&ta,callback))
	  {
	   case -1:
	     substdio_putsflush(subfdout,"out of memory\n");
	     break;
	   case 0:
	     substdio_putsflush(subfdout,"illegal address\n");
	     break;
	   default:
             if (token822_unparse(&sa,&taout,0) == -1)
	       substdio_putsflush(subfdout,"out of memory\n");
	     else
	      {
               substdio_puts(subfdout,"addrlist: ");
	       substdio_putflush(subfdout,sa.s,sa.len);
	      }
	  }
	}
    }
  }
}
