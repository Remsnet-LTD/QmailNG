#ifndef QQTALK_H
#define QQTALK_H

#include "substdio.h"

struct qqtalk
 {
  int flagerr;
  unsigned long pid;
  int fdm;
  int fde;
  substdio ss;
  char buf[1024];
 }
;

extern int qqtalk_open();
extern void qqtalk_put();
extern void qqtalk_puts();
extern void qqtalk_from();
extern void qqtalk_to();
extern void qqtalk_eput();
extern void qqtalk_fail();
extern int qqtalk_close();
extern unsigned long qqtalk_qp();

#define QQT_WAITPID -2
#define QQT_CRASHED -3
#define QQT_USAGE -4
#define QQT_BUG -5
#define QQT_SYS -6
#define QQT_READ -7
#define QQT_WRITE -8
#define QQT_NOMEM -9
#define QQT_EXECSOFT -11
#define QQT_TIMEOUT -13
#define QQT_TOOLONG -14

#endif
