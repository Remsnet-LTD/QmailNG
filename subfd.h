#ifndef SUBFD_H
#define SUBFD_H

extern struct substdio *subfdin;
extern struct substdio *subfdinsmall;
extern struct substdio *subfdout;
extern struct substdio *subfdoutsmall;
extern struct substdio *subfderr;

extern int subfd_read();
extern int subfd_readsmall();

#endif
