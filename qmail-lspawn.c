#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "wait.h"
#include "prot.h"
#include "substdio.h"
#include "case.h"
#include "qlx.h"
#include "exit.h"
#include "byte.h"
#include "str.h"
#include "fork.h"
#include "error.h"
#include "conf-home.h"
#include "conf-user.h"
#include "conf-unusual.h"
#include "auto-uids.h"

void initialize()
{
 ;
}

int truncreport = 3000;

void report(ss,wstat,s,len)
substdio *ss;
int wstat;
char *s;
int len;
{
 int i;
 if (wait_crashed(wstat))
  { substdio_puts(ss,"Zqmail-alias crashed.\n"); return; }
 switch(wait_exitcode(wstat))
  {
   case QLX_NOALIAS: substdio_puts(ss,"ZUnable to find alias user!\n"); return;
   case QLX_USAGE: substdio_puts(ss,"DInternal qmail-lspawn bug.\n"); return;
   case QLX_NFS: substdio_puts(ss,"ZNFS failure in qmail-alias.\n"); return;
   case QLX_EXECHARD: substdio_puts(ss,"DUnable to run qmail-alias.\n"); return;
   case QLX_EXECSOFT: substdio_puts(ss,"ZUnable to run qmail-alias.\n"); return;
   case QLX_SOFT: case 71: case 74: case 75:
     substdio_put(ss,"Z",1); break;
   case 0:
     substdio_put(ss,"K",1); break;
   default:
     substdio_put(ss,"D",1); break;
  }

 for (i = 0;i < len;++i) if (!s[i]) break;
 substdio_put(ss,s,i);
}

int spawn(fdmess,fdout,s,r,at)
int fdmess; int fdout;
char *s; char *r; int at;
{
 int f;

 if (!(f = fork()))
  {
   char username[LSPAWN_USERLEN];
   char *(args[10]);
   char *dash;
   char *extension;
   struct passwd *pw;
   struct stat st;
   
   r[at] = 0;

   if (!r[0]) _exit(0); /* <> */
   for (extension = r;*extension;++extension)
     if (*extension == LSPAWN_BREAK) break;

   dash = "";
   if (extension - r < sizeof(username))
    {
     if (*extension)
      {
       byte_copy(username,extension - r,r);
       username[extension - r] = 0;
       ++extension;
       dash = "-";
      }
     else str_copy(username,r);
     case_lowers(username);
     pw = getpwnam(username);
    }
   else
     pw = 0;
  
   if (pw)
    {
     if (!pw->pw_uid) pw = 0;
     else if (stat(pw->pw_dir,&st) == -1)
      { if (error_temp(errno)) _exit(QLX_NFS); pw = 0; }
     else if (st.st_uid != pw->pw_uid) pw = 0;
    }
  
   if (!pw) { extension = r; dash = "-"; pw = getpwnam(CONF_USERA); }
   if (!pw) _exit(QLX_NOALIAS);
  
   if (chdir(CONF_HOME) == -1) _exit(QLX_USAGE);
   if (prot_gid((int) pw->pw_gid) == -1) _exit(QLX_USAGE);
   if (prot_uid((int) pw->pw_uid) == -1) _exit(QLX_USAGE);

   close(0); dup(fdmess); close(fdmess);
   close(1); dup(fdout); close(fdout);
   close(2); dup(1);

   args[0] = "bin/qmail-alias";
   args[1] = "--";
   args[2] = pw->pw_name;
   args[3] = pw->pw_dir;
   args[4] = r;
   args[5] = dash;
   args[6] = extension;
   args[7] = r + at + 1;
   args[8] = s;
   args[9] = 0;

   execvp(*args,args);
   if (error_temp(errno)) _exit(QLX_EXECSOFT);
   _exit(QLX_EXECHARD);
  }
 return f;
}
