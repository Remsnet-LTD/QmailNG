#include <signal.h>
#include "signal.h"
#include "auto-hassgact.h"
#include "auto-hassgprm.h"

static void catch(sig,f)
int sig;
void (*f)();
{
#ifdef HASSIGACTION
 struct sigaction sa;
 sa.sa_handler = f;
 sa.sa_flags = 0;
 sigemptyset(&sa.sa_mask);
 sigaction(sig,&sa,(struct sigaction *) 0);
#else
 signal(sig,f); /* won't work under System V, even nowadays---dorks */
#endif
}

void signal_pause()
{
#ifdef HASSIGPROCMASK
 sigset_t ss;
 sigemptyset(&ss);
 sigsuspend(&ss);
#else
 sigpause(0);
#endif
}

void signal_blockchild()
{
#ifdef HASSIGPROCMASK
 sigset_t ss;
 sigemptyset(&ss);
 sigaddset(&ss,SIGCHLD);
 sigprocmask(SIG_SETMASK,&ss,(sigset_t *) 0);
#else
 sigsetmask(1 << (SIGCHLD - 1));
#endif
}

void signal_blocknone()
{
#ifdef HASSIGPROCMASK
 sigset_t ss;
 sigemptyset(&ss);
 sigprocmask(SIG_SETMASK,&ss,(sigset_t *) 0);
#else
 sigsetmask(0);
#endif
}

void signal_init() { catch(SIGPIPE,SIG_IGN); }
void signal_uninit() { catch(SIGPIPE,SIG_DFL); }
void signal_catchalarm(f) void (*f)(); { catch(SIGALRM,f); }
void signal_uncatchterm() { catch(SIGTERM,SIG_DFL); }
void signal_catchterm(f) void (*f)(); { catch(SIGTERM,f); }
void signal_uncatchchild() { catch(SIGCHLD,SIG_DFL); }
void signal_catchchild(f) void (*f)(); { catch(SIGCHLD,f); }

void signal_catchbug(f)
void (*f)();
{
 catch(SIGILL,f);
 catch(SIGABRT,f);
 catch(SIGFPE,f);
 catch(SIGBUS,f);
 catch(SIGSEGV,f);
#ifdef SIGSYS
 catch(SIGSYS,f);
#endif
#ifdef SIGEMT
 catch(SIGEMT,f);
#endif
}

void signal_ignoremisc()
{
 catch(SIGVTALRM,SIG_IGN);
 catch(SIGPROF,SIG_IGN);
 catch(SIGQUIT,SIG_IGN);
 catch(SIGINT,SIG_IGN);
 catch(SIGHUP,SIG_IGN);
#ifdef SIGXCPU
 catch(SIGXCPU,SIG_IGN);
#endif
#ifdef SIGXFSZ
 catch(SIGXFSZ,SIG_IGN);
#endif
}
