#ifndef CONSTMAP_H
#define CONSTMAP_H

typedef unsigned long constmap_hash;

struct constmap
 {
  int num;
  constmap_hash mask;
  constmap_hash *hash;
  int *first;
  int *next;
  char **input;
  int *inputlen;
 }
;

extern int constmap_init();
extern char *constmap();

#endif
