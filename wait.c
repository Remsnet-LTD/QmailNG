#include <sys/types.h>
#include <sys/wait.h>
#include "error.h"
#include "auto-haswaitp.h"
/* i don't think i have to worry about union wait or rusage. */

int wait_nohang(wstat) int *wstat;
{
#ifdef HASWAITPID
 return waitpid(-1,wstat,WNOHANG);
#else
 return wait3(wstat,WNOHANG,(struct rusage *) 0);
#endif
}

/* restriction: you must not care about any other child. */
int wait_pid(wstat,pid) int *wstat; int pid;
{
 int r;
 do
   r = wait(wstat);
 while ((r != pid) && ((r != -1) || (errno == error_intr)));
 return r;
}
