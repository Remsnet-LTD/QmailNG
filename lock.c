#include <sys/types.h>
#include <sys/file.h>
#include <fcntl.h>
#include "auto-hasflock.h"
#include "lock.h"

#ifdef HASFLOCK
int lock_ex(fd) int fd; { return flock(fd,LOCK_EX); }
int lock_un(fd) int fd; { return flock(fd,LOCK_UN); }
int lock_exnb(fd) int fd; { return flock(fd,LOCK_EX | LOCK_NB); }
#else
#define ULOCK 0 /* sigh */
#define LOCK 1
#define TLOCK 2
int lock_ex(fd) int fd; { return lockf(fd,LOCK,0); }
int lock_un(fd) int fd; { return lockf(fd,ULOCK,0); }
int lock_exnb(fd) int fd; { return lockf(fd,TLOCK,0); }
#endif
