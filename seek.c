#include <sys/types.h>
#include "seek.h"

#define SET 0 /* sigh */
#define CUR 1
#define END 2

int seek_trunc(fd,pos) int fd; seek_pos pos;
{ return ftruncate(fd,(off_t) pos); }

seek_pos seek_cur(fd) int fd;
{ return lseek(fd,(off_t) 0,CUR); }

int seek_end(fd) int fd;
{ if (lseek(fd,(off_t) 0,END) == -1) return -1; return 0; }

int seek_to(fd,pos) int fd; seek_pos pos;
{ if (lseek(fd,(off_t) pos,CUR) == -1) return -1; return 0; }

int seek_begin(fd) int fd;
{ if (lseek(fd,(off_t) 0,SET) == -1) return -1; return 0; }
