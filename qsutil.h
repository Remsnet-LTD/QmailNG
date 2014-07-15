#ifndef QSUTIL_H
#define QSUTIL_H

extern void wlog1(const char *);
extern void wlog2(const char *, const char *);
extern void wlog3(const char *, const char *, const char *);
extern void logsa(stralloc *);
extern void nomem(void);
extern void pausedir(const char *);
extern void logsafe(const char *);

#endif
