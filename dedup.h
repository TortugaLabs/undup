#ifndef _DEDUP_H
#define _DEDUP_H
#include "duptable.h"
#include "fscanner.h"
#include <sys/types.h>
#include <sys/stat.h>

struct dedup_cb {
  void (*do_dedup)(struct fs_dat *fs,ino_t *inos,int cnt,struct stat *stp,void *ext);
  void *ext;
};
struct duptab *dedup_cluster(struct duptab *in);
void dedup_pass(struct fs_dat *fs,struct dedup_cb *cb);
#endif
