#include "substdio.h"
#include "readwrite.h"
#include "exit.h"

char buf[16];
substdio out = SUBSTDIO_FDBUF(write,1,buf,sizeof(buf));

char host[256];

void main()
{
 host[0] = 0; /* sigh */
 gethostname(host,sizeof(host));
 host[sizeof(host) - 1] = 0;
 substdio_puts(&out,host);
 substdio_puts(&out,"\n");
 substdio_flush(&out);
 _exit(0);
}
