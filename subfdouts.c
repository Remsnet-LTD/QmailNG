#include "readwrite.h"
#include "substdio.h"
#include "subfd.h"

char subfd_outbufsmall[256];
static struct substdio outsmall = SUBSTDIO_FDBUF(write,1,subfd_outbufsmall,256);
struct substdio *subfdoutsmall = &outsmall;
