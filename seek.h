#ifndef SEEK_H
#define SEEK_H

typedef unsigned long seek_pos;

extern seek_pos seek_cur();

extern int seek_begin();
extern int seek_to();
extern int seek_end();

extern int seek_trunc();

#endif
