/*
 * Does the actual de-duplication
 */
#include "fscanner.h"
#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "calchash.h"
#include "hcache.h"
#include <stdlib.h>
#include <stdio.h>

#define FIRST_BYTES	4096

struct fscanner_dat *gfs;

/* Sorts inodes so oldest is first */
static int dedup_cmp(const void *a,const void *b) {
  ino_t ino_a = *((ino_t *)a);
  ino_t ino_b = *((ino_t *)b);
  struct fscanner_dat *fs = gfs;
  time_t mtime_a, mtime_b;

  if (!inodetab_get(fs->itab,ino_a,&mtime_a)) return 0;
  if (!inodetab_get(fs->itab,ino_b,&mtime_b)) return 0;

  if (mtime_a < mtime_b) return -1;
  if (mtime_a > mtime_b) return 1;
  return 0;
}


static void do_dedup(struct fscanner_dat *fs,ino_t *inos, int icnt,struct stat *stp) {
  int i;
  char **fpt, *basepath, *vpath;
  struct stat stb;
  gfs = fs;
  ckpt(0);
  qsort(inos,icnt,sizeof(ino_t),dedup_cmp);
  ckpt("%llx %llx\n", (long long)inos[0],(long long)inos[1]);

  fpt = inodetab_get(fs->itab,inos[0],&stp->st_mtime);
  stb.st_ino = inos[0];

  if (fpt == NULL || *fpt==NULL) fatal(ENOENT,"Missing i-node %llx",(long long)inos[0]);
  basepath = mystrcat(fs->root,"/",*fpt,NULL);
  ckpt(0);
  for (i = 1; i < icnt; i++) {
    fpt = inodetab_get(fs->itab,inos[i],&stp->st_mtime);
    if (fpt == NULL || *fpt==NULL) fatal(ENOENT,"Missing i-node %llx",(long long)inos[i]);
    ckpt("%llx %s\n",(long long)inos[i],*fpt);
    while (*fpt) {
      vpath = mystrcat(fs->root,"/",*(fpt++),NULL);
      ckpt("%s\n",vpath);
      if (stb.st_ino != inos[i]) {
	/* Compute statistics... */
	if (lstat(vpath,&stb) == -1) errorexit("lstat(%s)",vpath);
	fs->blocks += stb.st_blocks;
      }
      fs->files++;
      vmsg("Linking %s -> %s\n", vpath, basepath);
      if (!gopts.dryrun) {
	if (unlink(vpath)==-1) errorexit("unlink(%s)",vpath);
	if (link(basepath,vpath)==-1) errorexit("link(%s->%s)",basepath,vpath);
      }
      free(vpath);
    }
  }
  free(basepath);
}


static void dedup2b(struct fscanner_dat *fs,struct duptab *dups) {
  ino_t *inos;
  int icnt;
  struct stat st;

  for (inos = duptab_first(dups,&icnt,&st);
       inos != NULL ;
       inos = duptab_next(dups,&icnt,&st)) {
    if (icnt < 2) continue;
    ckpt("%llx (%d)\n",(long long)*inos, icnt);
    do_dedup(fs, inos, icnt, &st);
  }
}

static void dedup2a(struct fscanner_dat *fs,ino_t *inos, int icnt,struct stat *stp) {
  struct duptab *dups = duptab_new();
  int i;
  char **fpt, *hash, *vn;

  for (i=0; i < icnt; i++) {
    stp->st_ino = inos[i];
    fpt = inodetab_get(fs->itab,inos[i],&stp->st_mtime);
    if (fpt == NULL || *fpt==NULL) fatal(ENOENT,"Missing i-node %llx",(long long)inos[i]);

    if (fs->cache) {
      hcache_get(fs->cache,stp,&hash);
      if (hash == NULL) {
	// Compute and cache hash
	vn = mystrcat(fs->root,"/",*fpt,NULL);
	hash = hash_file(vn);
	hcache_put(fs->cache,stp,hash,hash_len());
	free(vn);
      }
    } else {
      vn = mystrcat(fs->root,"/",*fpt,NULL);
      hash = hash_file(vn);
      free(vn);
    }
    duptab_add(dups, stp, hash_len(), hash);
    if (hash) free(hash);
  }
  duptab_dump(dups);

  dedup2b(fs, dups);
  duptab_free(dups);
}

static void dedup1b(struct fscanner_dat *fs,struct duptab *dups) {
  ino_t *inos;
  int icnt;
  struct stat st;

  for (inos = duptab_first(dups,&icnt,&st);
       inos != NULL ;
       inos = duptab_next(dups,&icnt,&st)) {
    if (icnt < 2) continue;
    dedup2a(fs, inos, icnt, &st);
  }
}

static void dedup1a(struct fscanner_dat *fs,ino_t *inos, int icnt,struct stat *stp) {
  struct duptab *dups = duptab_new();
  int i, fd, len;
  char **fpt, *vn, buf[FIRST_BYTES];

  for (i=0; i < icnt; i++) {
    stp->st_ino =inos[i];
    fpt = inodetab_get(fs->itab,inos[i],&stp->st_mtime);
    if (fpt == NULL || *fpt==NULL) fatal(ENOENT,"Missing i-node %llx",(long long)inos[i]);
    vn = mystrcat(fs->root,"/",*fpt,NULL);
    if ((fd = open(vn,O_RDONLY)) != -1) {
      len = read(fd,buf,FIRST_BYTES);
      switch (len) {
      case 0:
	fatal(ENODATA,"read(%s): no data",vn);
      case -1:
	errorexit("read(%s)",vn);
      }
      close(fd);
    } else errorexit("open(%s)", vn);
    free(vn);
    duptab_add(dups, stp, len, buf);
  }
  dedup1b(fs, dups);
  duptab_free(dups);
}

void dedup(struct fscanner_dat *fs) {
  ino_t *inodes;
  int icnt;
  struct stat st;
  duptab_sort(fs->dtab);

  for (inodes = duptab_first(fs->dtab,&icnt,&st) ;
       inodes != NULL ;
       inodes = duptab_next(fs->dtab,&icnt,&st)) {
    if (icnt < 2) continue;
    ckpt("DEDUP: %llx (%d)\n", (long long)*inodes, icnt);
    dedup1a(fs,inodes,icnt,&st);
  }
}
