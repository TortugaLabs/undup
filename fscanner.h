#ifndef _FSCANNER_H
#define _FSCANNER_H
#include "inodetab.h"
#include "duptable.h"
#include "hcache.h"
#include <sys/types.h>
#include <sys/stat.h>

struct fs_dat {
  struct inodetab *itab;
  struct duptab *dtab;
  struct hcache *cache;
  char *root;
};

struct cat_cb {
  void (*callback)(char *dir,char *file, struct stat *stdat,void *ext);
  void *ext;
};

void fscanner_init(struct fs_dat *fs,char *root,int usecache);
void fscanner(struct fs_dat *dat, struct cat_cb *cb);
void fscanner_close(struct fs_dat *scandat);


#endif
