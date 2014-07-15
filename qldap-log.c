#include "substdio.h"
#include "qldap-log.h"

/* level 0 = no logging
         1 = fatal errors
         2 = connection setup and warnings
         3 = verbose */

int loglevel = 0;

void logpid(level) int level;
{
  char pidstring[FMT_ULONG];
  if (level > loglevel) return;
  substdio_puts(subfderr,"qmail-smtpd ");
  pidstring[fmt_ulong(pidstring,(unsigned long) getpid())] = 0;
  substdio_puts(subfderr,pidstring);
  substdio_puts(subfderr,": ");
}

void logline(level,string) int level; char *string;
{
  if (level > loglevel) return;
  logpid();
  substdio_puts(subfderr,string);
  substdio_puts(subfderr,"\n");
  substdio_flush(subfderr);
}

void logstring(level,string) int level; char *string;
{
  if (level > loglevel) return;
  substdio_puts(subfderr,string);
  substdio_puts(subfderr," ");
}

void logflush(level) int level;
{
  if (level > loglevel) return;
  substdio_puts(subfderr,"\n");
  substdio_flush(subfderr);
}
