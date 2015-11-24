#include "utils.h"
#include "hcache.h"
#include <gdbm.h>
#include <unistd.h>
#include <stdio.h>

struct hcache {
  GDBM_FILE dbf;
  GDBM_FILE validated;
  char *path;
  int type;
  int hlen;
  int hits;
  int misses;
};

struct hcache_key {
  ino_t inode;
  mode_t mode;
  uid_t uid;
  gid_t gid;
  off_t size;
  time_t mtime;
  int cktype;
};

static datum _hcache_genkey(struct hcache *cache,struct stat *st) {
  datum key;
  struct hcache_key *sk = mymalloc(sizeof(struct hcache_key));
  memset(sk,0,sizeof(struct hcache_key));
  key.dsize = sizeof(struct hcache_key);
  sk->inode = st->st_ino;
  sk->mode = st->st_mode & ~S_IFMT;
  sk->uid = st->st_uid;
  sk->gid = st->st_gid;
  sk->size = st->st_size;
  sk->mtime = st->st_mtime;
  sk->cktype = cache->type;
  key.dptr = (char *)sk;
  //ckptm("i:%llx m:%03o s:%lld %lld %c\n",(long long)sk->inode,sk->mode,(long long)sk->size,(long long)sk->mtime, sk->cktype);
  return key;
}

const char *hcache_getpath(struct hcache *cache) {
  return cache->path;
}

void hcache_stats(struct hcache *cache,int *hits, int *misses){
  //ckpt();
  if(hits) *hits = cache->hits;
  if(misses) *misses = cache->misses;
}

struct hcache *hcache_new(const char *base,int type,int len) {
  struct hcache *cache = (struct hcache *)mymalloc(sizeof(struct hcache));
  char *cachefile = mystrcat(base,".hcd");

  memset(cache,0,sizeof(struct hcache));
  cache->path = cachefile;
  cache->dbf = gdbm_open(cachefile,0,GDBM_READER,0666,NULL);
  cache->type = type;
  cache->hlen = len;
  return cache;
}

static void _hcache_validate_done(struct hcache *cache) {
  char *vfname;
  if (!cache->validated) return;
  gdbm_close(cache->validated);
  vfname = mystrcat(cache->path,".validate");
  if (cache->dbf) {
    gdbm_close(cache->dbf);
    unlink(cache->path);
  }
  if (rename(vfname,cache->path) == -1)
    errormsg("rename(%s->%s)",vfname, cache->path);
  free(vfname);
  cache->validated = NULL;
  cache->dbf = gdbm_open(cache->path,0, GDBM_WRITER, 0666,NULL);
}
static void _hcache_validate_init(struct hcache *cache) {
  char *vfname;
  if (cache->validated) return;
  vfname = mystrcat(cache->path,".validate");
  cache->validated = gdbm_open(vfname,0, GDBM_NEWDB, 0666,NULL);
  if (!cache->validated)
    fatal(gdbm_errno,"gdbmopen(%s): %s", vfname, gdbm_strerror(gdbm_errno));
  free(vfname);
}

struct hcache *hcache_free(struct hcache *cache) {
  _hcache_validate_done(cache);
  if (cache->dbf) gdbm_close(cache->dbf);
  free(cache->path);
  free(cache);
  return NULL;
}
void hcache_validate(struct hcache *cache, struct stat *st) {
  datum key, datum;
  //ckpt(0);
  _hcache_validate_init(cache);
  if (!cache->dbf) return;
  //ckpt(0);
  key = _hcache_genkey(cache,st);
  //ckpt(0);
  datum = gdbm_fetch(cache->dbf, key);
  //ckpt(0);
  if (datum.dptr == NULL) {
    free(key.dptr);
    return;
  }
  if (gdbm_store(cache->validated, key, datum, GDBM_INSERT) == -1)
    fatal(gdbm_errno,"gdbm_store %s", gdbm_strerror(gdbm_errno));
  //ckpt(0);
  free(datum.dptr);
  free(key.dptr);
}
void hcache_put(struct hcache *cache, struct stat *st,char *hash) {
  datum key, datum;
  //ckpt();
  _hcache_validate_done(cache);
  key = _hcache_genkey(cache,st);
  //ckptm(">>");
  //printhex(stderr,key.dptr,key.dsize,0);
  //ckptm("\n");

  datum.dptr = hash;
  datum.dsize = cache->hlen;
  //ckpt();
  //ckptm("hash=%s, len=%d\n",hash,cache->hlen);
  if (gdbm_store(cache->dbf,key,datum,GDBM_REPLACE) == -1)
    fatal(gdbm_errno,"gdbm_store %s", gdbm_strerror(gdbm_errno));
  //ckpt();
  free(key.dptr);
  //ckpt();
}

int hcache_get(struct hcache *cache, struct stat *st,char **hash) {
  datum key, datum;
  _hcache_validate_done(cache);
  key = _hcache_genkey(cache,st);
  datum = gdbm_fetch(cache->dbf, key);
  free(key.dptr);
  if (hash) *hash = datum.dptr;
  if (datum.dptr) {
    cache->hits++;
    //printhex(stderr,datum.dptr, datum.dsize,0); fprintf(stderr,"\n");
  } else {
    cache->misses++;
  }
  return datum.dptr != NULL;
}

void hcache_del(struct hcache *cache, struct stat *st) {
  datum key;
  _hcache_validate_done(cache);
  key = _hcache_genkey(cache,st);
  gdbm_delete(cache->dbf,key);
  free(key.dptr);
}
