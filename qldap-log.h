#ifndef __QLDAP_LOG_H__
#define __QLDAP_LOG_H__

/* level 0 = no logging
         1 = fatal errors
         2 = connection setup and warnings
         3 = verbose */

void logpid(level) int level;
void logline(level,string) int level; char *string;
void logstring(level,string) int level; char *string;
void logflush(level) int level;

#endif
