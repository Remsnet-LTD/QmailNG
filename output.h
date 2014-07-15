/* qldap-debug.h, jeker@n-r-g.com, best viewed with tabsize = 4 */
#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include "substdio.h"
#include <stdarg.h>

#define STDERR 2
#define STDOUT 1
#define STDIN  0

void va_output(substdio *ss, char *fmt, va_list args);
/* works like printf has the format options %i, ...
 * all flags (#, 0, -, ' ', +, ' ... ) are not supported
 * Also not supported are all options for foating-point numbers
 * (not needed in qmail)
 * Supported conversion specifiers: diuxcsSp%
 * diux are for integer (long) conversions (di are signed all other unsigned)
 * c is a single unsigned char
 * s is a zero terminated string
 * S is a stralloc object (should not be zero terminated (else the zero
 *   will be printed))
 * p is the hex address of a generic pointer (void *)
 * % is the % sign */
void output(substdio *ss, char *fmt, ...);

#endif
