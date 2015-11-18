#ifndef _FSCANNER_H
#define _FSCANNER_H
#include "inodetab.h"
#include "duptable.h"
#include "hcache.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include "calchash.h"
#include <stdarg.h>

#define DFT_CATFMT "%d %s %03o n:%d u:%d g:%d s:%d b:%d ts:%d %s%s%s\n"

struct opts_t {
  FILE *catfp;
  const char *catfmt;
  int dryrun;
  int usecache;
  int mstats;
  int verbose;
  int scanonly;
};
extern struct opts_t gopts;
static inline void gopts_init(struct opts_t *p) {
  memset(p,0,sizeof(*p));
  p->catfmt = DFT_CATFMT;
  p->dryrun = true;
  p->usecache = true;
}

struct fscanner_dat {
  struct inodetab *itab;
  struct duptab *dtab;
  struct hcache *cache;
  char *root;
  int blocks;
  int files;
};

static inline void fscanner_init(struct fscanner_dat *fs,char *root) {
  fs->root = root;
  fs->itab = inodetab_new();
  fs->dtab = duptab_new();

  if (gopts.usecache) {
    char *cachefile = mystrcat(root,".hcf",NULL);
    fs->cache = hcache_new(cachefile,hash_type());
    free(cachefile);
  } else {
    fs->cache = NULL;
  }
  fs->blocks = 0;
  fs->files = 0;
}

void fscanner(struct fscanner_dat *scandat);
void dedup(struct fscanner_dat *fs);
void fscanner_close(struct fscanner_dat *scandat);

#define vmsg(...) if (gopts.verbose) fprintf(stderr,__VA_ARGS__)
#endif
