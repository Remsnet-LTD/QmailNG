#include "substdio.h"
#include "subfd.h"
#include "readwrite.h"
#include "exit.h"
#include "fmt.h"
#include "conf-home.h"
#include "conf-unusual.h"

char num[FMT_ULONG];

void main()
{
 substdio_put(subfdoutsmall,num,fmt_ulong(num,QFN_SPLIT));
 substdio_puts(subfdoutsmall,"\n");
 substdio_flush(subfdoutsmall);
 _exit(0);
}
