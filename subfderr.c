#include "readwrite.h"
#include "substdio.h"
#include "subfd.h"

char subfd_errbuf[256];
static struct substdio err = SUBSTDIO_FDBUF(write,2,subfd_errbuf,256);
struct substdio *subfderr = &err;
