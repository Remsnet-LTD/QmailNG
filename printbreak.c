#include "substdio.h"
#include "subfd.h"
#include "readwrite.h"
#include "exit.h"
#include "conf-home.h"
#include "conf-unusual.h"

void main()
{
 char ch;
 ch = USEREXT_BREAK;
 substdio_put(subfdoutsmall,&ch,1);
 substdio_puts(subfdoutsmall,"\n");
 substdio_flush(subfdoutsmall);
 _exit(0);
}
