#ifndef CONF_UNUSUAL_H
#define CONF_UNUSUAL_H

#define SPAWN_NUMD 120 /* silent concurrency limit */

#define USEREXT_BREAK '-' /* delimiter in user-ext */

#define ALIAS_EMPTY "./Mailbox\n"

#define ALIAS_PATERNALISM 022 /* stat bits not allowed in ~ and ~/.qmail */
/* Note that ~ftp, ~www, ~uucp, etc. should be owned by root. */

#define QFN_SPLIT 23 /* queue subdirectory split */

#define GETPW_USERLEN 32

#endif
