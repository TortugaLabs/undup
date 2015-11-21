#ifndef _UNDUP_H
#define _UNDUP_H
#include <stdio.h>

struct undup_opts {
  int dryrun;
  int usecache;
  int mstats;
  int verbose;
  int scanonly;
};
extern struct undup_opts gopts;

int undup_main(int argc,char **argv);
#define vmsg(...) if (gopts.verbose) fprintf(stderr,__VA_ARGS__)
#endif
