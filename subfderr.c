#include "readwrite.h"
#include "substdio.h"
#include "subfd.h"

char subfd_errbuf[64];
static struct substdio err = SUBSTDIO_FDBUF(write,2,subfd_errbuf,64);
struct substdio *subfderr = &err;
