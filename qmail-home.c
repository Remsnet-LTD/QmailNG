#include "substdio.h"
#include "subfd.h"
#include "readwrite.h"
#include "exit.h"
#include "conf-home.h"

void main()
{
 substdio_puts(subfdoutsmall,CONF_HOME);
 substdio_putsflush(subfdoutsmall,"\n");
 _exit(0);
}
