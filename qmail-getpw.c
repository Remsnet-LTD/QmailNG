#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "readwrite.h"
#include "substdio.h"
#include "error.h"
#include "qlx.h"
#include "exit.h"
#include "byte.h"
#include "str.h"
#include "case.h"
#include "fmt.h"
#include "conf-user.h"
#include "conf-unusual.h"

char *local;
struct passwd *pw;
char *dash;
char *extension;

int userext()
{
  char username[GETPW_USERLEN];
  struct stat st;

  for (extension = local;*extension;++extension)
    if (*extension == USEREXT_BREAK)
      break;
  dash = "";

  if (extension - local >= sizeof(username)) return 0;

  byte_copy(username,extension - local,local);
  username[extension - local] = 0;
  if (*extension) {
    ++extension;
    dash = "-";
  }

  case_lowers(username);
  pw = getpwnam(username);

  if (!pw) return 0;
  if (!pw->pw_uid) return 0;

  if (stat(pw->pw_dir,&st) == -1) {
    if (error_temp(errno)) _exit(QLX_NFS);
    return 0;
  }

  return (st.st_uid == pw->pw_uid);
}

char outbuf[256];
struct substdio out = SUBSTDIO_FDBUF(write,1,outbuf,256);
char num[FMT_ULONG];

void main(argc,argv)
int argc;
char **argv;
{
  local = argv[1];
  if (!local) _exit(1);

  if (!userext()) {
    extension = local;
    dash = "-";
    pw = getpwnam(CONF_USERA);
  }

  if (!pw) _exit(QLX_NOALIAS);

  substdio_puts(&out,pw->pw_name);
  substdio_put(&out,"",1);
  substdio_put(&out,num,fmt_ulong(num,(long) pw->pw_uid));
  substdio_put(&out,"",1);
  substdio_put(&out,num,fmt_ulong(num,(long) pw->pw_gid));
  substdio_put(&out,"",1);
  substdio_puts(&out,pw->pw_dir);
  substdio_put(&out,"",1);
  substdio_puts(&out,dash);
  substdio_put(&out,"",1);
  substdio_puts(&out,extension);
  substdio_put(&out,"",1);
  substdio_flush(&out);

  _exit(0);
}
