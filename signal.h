#ifndef SIGNAL_H
#define SIGNAL_H

extern void signal_init();
extern void signal_uninit();
extern void signal_ignoremisc();
extern void signal_catchbug();
extern void signal_catchterm();
extern void signal_uncatchterm();
extern void signal_catchalarm();
extern void signal_catchchild();
extern void signal_uncatchchild();
extern void signal_blocknone();
extern void signal_blockchild();
extern void signal_pause();

#endif
