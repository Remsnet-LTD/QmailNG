#include "prot.h"
#include "exit.h"
#include "fork.h"
#include "auto-uids.h"

char *(qsargs[]) = { "qmail-send", 0 };
char *(qcargs[]) = { "qmail-clean", 0 };
char *(qlargs[]) = { "qmail-lspawn", 0 };
char *(qrargs[]) = { "qmail-rspawn", 0 };

void die() { _exit(1); }

void main(argc,argv)
int argc;
char **argv;
{
 int pi0[2];
 int pi1[2];
 int pi2[2];
 int pi3[2];
 int pi4[2];
 int pi5[2];
 int pi6[2];

 if (chdir("/") == -1) die();
 umask(077);
 if (prot_gid(GID_QMAIL) == -1) _exit(1);

 close(2); dup(0);
 close(3); dup(0);
 close(4); dup(0);
 close(5); dup(0);
 close(6); dup(0);

 if (argv[1])
  {
   if (pipe(pi0) == -1) die();
   switch(fork()) { case -1: die(); case 0:
     close(0); dup(pi0[0]);
     close(2); close(3); close(4); close(5); close(6);
     close(pi0[0]); close(pi0[1]);
     execvp(argv[1],argv + 1); _exit(1); }
   close(1); dup(pi0[1]);
   close(pi0[0]); close(pi0[1]);
  }

 if (pipe(pi1) == -1) die();
 if (pipe(pi2) == -1) die();
 if (pipe(pi3) == -1) die();
 if (pipe(pi4) == -1) die();
 if (pipe(pi5) == -1) die();
 if (pipe(pi6) == -1) die();

 switch(fork()) { case -1: die(); case 0:
     close(0); dup(pi1[0]);
     close(1); dup(pi2[1]);
     close(2); close(3); close(4); close(5); close(6);
     close(pi1[0]); close(pi1[1]); close(pi2[0]); close(pi2[1]);
     close(pi3[0]); close(pi3[1]); close(pi4[0]); close(pi4[1]);
     close(pi5[0]); close(pi5[1]); close(pi6[0]); close(pi6[1]);
     execvp(*qlargs,qlargs); _exit(1); }

 switch(fork()) { case -1: die(); case 0:
     close(0); dup(pi3[0]);
     close(1); dup(pi4[1]);
     close(2); close(3); close(4); close(5); close(6);
     close(pi1[0]); close(pi1[1]); close(pi2[0]); close(pi2[1]);
     close(pi3[0]); close(pi3[1]); close(pi4[0]); close(pi4[1]);
     close(pi5[0]); close(pi5[1]); close(pi6[0]); close(pi6[1]);
     if (prot_uid(UID_REMOTE) == -1) _exit(1);
     execvp(*qrargs,qrargs); _exit(1); }

 switch(fork()) { case -1: die(); case 0:
     close(0); dup(pi5[0]);
     close(1); dup(pi6[1]);
     close(2); close(3); close(4); close(5); close(6);
     close(pi1[0]); close(pi1[1]); close(pi2[0]); close(pi2[1]);
     close(pi3[0]); close(pi3[1]); close(pi4[0]); close(pi4[1]);
     close(pi5[0]); close(pi5[1]); close(pi6[0]); close(pi6[1]);
     if (prot_uid(UID_QUEUE) == -1) _exit(1);
     execvp(*qcargs,qcargs); _exit(1); }

 close(0); dup(1);
 close(1); dup(pi1[1]);
 close(2); dup(pi2[0]);
 close(3); dup(pi3[1]);
 close(4); dup(pi4[0]);
 close(5); dup(pi5[1]);
 close(6); dup(pi6[0]);
 close(pi1[0]); close(pi1[1]); close(pi2[0]); close(pi2[1]);
 close(pi3[0]); close(pi3[1]); close(pi4[0]); close(pi4[1]);
 close(pi5[0]); close(pi5[1]); close(pi6[0]); close(pi6[1]);
 if (prot_uid(UID_SEND) == -1) _exit(1);
 execvp(*qsargs,qsargs);
 _exit(1);
}
