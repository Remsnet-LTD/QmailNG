#include "substdio.h"
#include "readwrite.h"
#include "exit.h"
#include "fmt.h"
#include "conf-home.h"
#include "conf-unusual.h"

char num[FMT_ULONG];
char buf[16];
substdio out = SUBSTDIO_FDBUF(write,1,buf,sizeof(buf));

void main()
{
 substdio_put(&out,num,fmt_ulong(num,QFN_SPLIT));
 substdio_puts(&out,"\n");
 substdio_flush(&out);
 _exit(0);
}
