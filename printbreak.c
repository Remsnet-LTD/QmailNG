#include "substdio.h"
#include "readwrite.h"
#include "exit.h"
#include "conf-home.h"
#include "conf-unusual.h"

char buf[16];
substdio out = SUBSTDIO_FDBUF(write,1,buf,sizeof(buf));

void main()
{
 char ch;
 ch = USEREXT_BREAK;
 substdio_put(&out,&ch,1);
 substdio_puts(&out,"\n");
 substdio_flush(&out);
 _exit(0);
}
