/*
 * Does the actual de-duplication
 */
#include "fscanner.h"
#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "calchash.h"
#include "hcache.h"
#include <stdlib.h>
#include <stdio.h>

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
  qsort(inos,icnt,sizeof(ino_t),dedup_cmp);

  fpt = inodetab_get(fs->itab,inos[0],&stp->st_mtime);
  stb.st_ino = inos[0];

  if (fpt == NULL || *fpt==NULL) fatal(ENOENT,"Missing i-node %d",inos[0]);
  basepath = mystrcat(fs->root,"/",*fpt,NULL);

  for (i = 1; i < icnt; i++) {
    fpt = inodetab_get(fs->itab,inos[i],&stp->st_mtime);
    if (fpt == NULL || *fpt==NULL) fatal(ENOENT,"Missing i-node %d",inos[i]);
    while (*fpt) {
      vpath = mystrcat(fs->root,"/",*(fpt++));
      if (stb.st_ino != inos[i]) {
	/* Compute statistics... */
	if (lstat(vpath,&stb) == -1) errorexit("lstat(%s)",vpath);
	fs->blocks += stb.st_blocks;
      }
      fs->files++;
      if (fs->dryrun) {
	fprintf(stderr,"Linking %s -> %s\n", vpath, basepath);
      } else {
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
    do_dedup(fs, inos, icnt, &st);
  }
}

static void dedup2a(struct fscanner_dat *fs,ino_t *inos, int icnt,struct stat *stp) {
  struct duptab *dups = duptab_new();
  int i;
  char **fpt, *phash, *fhash;

  for (i=0; i < icnt; i++) {
    fpt = inodetab_get(fs->itab,inos[i],&stp->st_mtime);
    if (fpt == NULL || *fpt==NULL) fatal(ENOENT,"Missing i-node %d",inos[i]);

    if (fs->cache) {
      hcache_get(fs->cache,stp,&phash,&fhash,hash_len());
      if (fhash == NULL) {
	// Compute and cache hash
	fhash = hash_full(*fpt,fs->root);
	hcache_put(fs->cache,stp,phash,fhash,hash_len());
      }
      if (phash) free(phash);
    } else {
      fhash = hash_full(*fpt,fs->root);
    }
    duptab_add(dups, stp, hash_len(), fhash, NULL);
    if (fhash) free(fhash);

  }
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
  int i;
  char **fpt, *hash, *ohash;

  for (i=0; i < icnt; i++) {
    fpt = inodetab_get(fs->itab,inos[i],&stp->st_mtime);
    if (fpt == NULL || *fpt==NULL) fatal(ENOENT,"Missing i-node %d",inos[i]);

    if (fs->cache) {
      hcache_get(fs->cache,stp,&hash,&ohash,hash_len());
      if (hash == NULL) {
	// Compute and cache hash
	hash = hash_part(*fpt,fs->root);
	hcache_put(fs->cache,stp,hash,ohash,hash_len());
      }
      if (ohash) free(ohash);
    } else {
      hash = hash_part(*fpt,fs->root);
    }
    duptab_add(dups, stp, hash_len(), hash, NULL);
    if (hash) free(hash);

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
    dedup1a(fs,inodes,icnt,&st);
  }
}
