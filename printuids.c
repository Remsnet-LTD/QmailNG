#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include "subfd.h"
#include "substdio.h"
#include "fmt.h"
#include "exit.h"
#include "conf-user.h"

void fatal(type,name) char *type; char *name;
{
 substdio_puts(subfderr,"fatal: can't find ");
 substdio_puts(subfderr,type);
 substdio_puts(subfderr," ");
 substdio_puts(subfderr,name);
 substdio_putsflush(subfderr,"\n");
 _exit(1);
}

void define(s,d) char *s; int d;
{
 char strd[FMT_ULONG];
 strd[fmt_ulong(strd,(unsigned long) d)] = 0;
 substdio_puts(subfdout,"#define ");
 substdio_puts(subfdout,s);
 substdio_puts(subfdout," ");
 substdio_puts(subfdout,strd);
 substdio_puts(subfdout,"\n");
}

void user(s,u) char *s; char *u;
{
 struct passwd *pw;
 pw = getpwnam(u);
 if (!pw) fatal("user",u);
 define(s,(int) pw->pw_uid);
}

void group(s,g) char *s; char *g;
{
 struct group *gr;
 gr = getgrnam(g);
 if (!gr) fatal("group",g);
 define(s,(int) gr->gr_gid);
}

void main()
{
 substdio_puts(subfdout,"#ifndef AUTO_UIDS_H\n#define AUTO_UIDS_H\n");
 user("UID_OWNER",CONF_USERO);
 user("UID_ALIAS",CONF_USERA);
 user("UID_SMTPD",CONF_USERD);
 user("UID_QUEUE",CONF_USERQ);
 user("UID_REMOTE",CONF_USERR);
 user("UID_SEND",CONF_USERS);
 group("GID_QMAIL",CONF_GROUPQ);
 group("GID_NOBODY",CONF_GROUPX);
 substdio_putsflush(subfdout,"#endif\n");
 _exit(0);
}
