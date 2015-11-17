#ifndef _FSCANNER_H
#define _FSCANNER_H
#include "inodetab.h"
#include "duptable.h"
#include "hcache.h"
#include <stdio.h>

struct fscanner_dat {
  struct inodetab *itab;
  struct duptab *dtab;
  struct hcache *cache;
  char *root;
  FILE *catfp;
  const char *catfmt;
  int blocks;
  int dryrun;
  int files;
};

void fscanner_init(struct fscanner_dat *scandat,char *root,const char *cat,const char *cahefile, int type,int dryrun);
void fscanner(struct fscanner_dat *scandat);
void dedup(struct fscanner_dat *fs);
void fscanner_close(struct fscanner_dat *scandat);

#define DFT_CATFMT "%d %s %03o n:%d u:%d g:%d s:%d b:%d ts:%d %s%s%s\n"

#endif
