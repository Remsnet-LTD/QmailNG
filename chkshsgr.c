#include "exit.h"
void main()
{
 short x[4];

 x[0] = x[1] = 1;
 if (getgroups(1,x) == 0) if (setgroups(1,x) == -1) _exit(1);
 _exit(0);
}
