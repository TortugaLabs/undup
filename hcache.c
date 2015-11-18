#include "utils.h"
#include "packt.h"
#include "hcache.h"
#include <gdbm.h>
#include <unistd.h>
#include <stdio.h>

#define HCACHE_NONE 0x00
#define HCACHE_PART 0x01
#define HCACHE_FULL 0x02

struct hcache {
  GDBM_FILE dbf;
  GDBM_FILE validated;
  char *path;
  char ktempl[32];
  int type;
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
// cache_data (byte).short_hash.long_hash
//<inode> : <mtime>.<uid>.<gid>.<mode>.<size>.<cksum.type>.<cksum.4k>.<cksum.type>.<cksum>
//<uid>.<gid>.<mode>.<size> | <cksum.4k> | <cksum>

static datum _hcache_genkey(struct hcache *cache,struct stat *st) {
  datum key;
  //ckpt("st:%lld %03o/%d %d %d\n",
  //(long long)st->st_ino,
  //st->st_mode & ~S_IFMT,st->st_mode & ~S_IFMT,
  //st->st_uid,
  //st->st_gid);
  /*
  key.dptr = packt(&key.dsize,cache->ktempl,
		   st->st_ino,
		   st->st_mode & ~S_IFMT,
		   st->st_uid,
		   st->st_gid,
		   st->st_size,
		   st->st_mtime,
		   cache->type);
  */
  struct hcache_key *sk = mymalloc(sizeof(struct hcache_key));
  sk->inode = st->st_ino;
  sk->mode = st->st_mode & ~S_IFMT;
  sk->uid = st->st_uid;
  sk->gid = st->st_gid;
  sk->size = st->st_size;
  sk->mtime = st->st_mtime;
  sk->cktype = cache->type;
  //ckpt(0);
  key.dptr = (char *)sk;
  return key;
}

static void _hcache_init_ktempl(char *p) {
  *(p++) = '0' + sizeof(ino_t);
  *(p++) = '0' + sizeof(mode_t);
  *(p++) = '0' + sizeof(uid_t);
  *(p++) = '0' + sizeof(gid_t);
  *(p++) = '0' + sizeof(off_t);
  *(p++) = '0' + sizeof(time_t);
  *(p++) = '1';
  *p = 0;
}

struct hcache *hcache_new(const char *base,int type) {
  struct hcache *cache = (struct hcache *)mymalloc(sizeof(struct hcache));
  char *cachefile = mystrcat(base,".hcd",NULL);

  memset(cache,0,sizeof(struct hcache));
  cache->path = cachefile;
  cache->dbf = gdbm_open(cachefile,0,GDBM_READER,0666,NULL);
  cache->type = type;
  _hcache_init_ktempl(cache->ktempl);

  return cache;
}

static void _hcache_validate_done(struct hcache *cache) {
  char *vfname;
  if (!cache->validated) return;
  gdbm_close(cache->validated);
  vfname = mystrcat(cache->path,".validate",NULL);
  if (cache->dbf) {
    gdbm_close(cache->dbf);
    unlink(cache->path);
  }
  if (rename(vfname,cache->path) == -1)
    errorexit("rename(%s->%s)",vfname, cache->path);
  free(vfname);
  cache->validated = NULL;
  cache->dbf = gdbm_open(cache->path,0, GDBM_WRITER, 0666,NULL);
}
static void _hcache_validate_init(struct hcache *cache) {
  char *vfname;
  if (cache->validated) return;
  vfname = mystrcat(cache->path,".validate",NULL);
  cache->validated = gdbm_open(vfname,0, GDBM_NEWDB, 0666,NULL);
  if (!cache->validated)
    fatal(gdbm_errno,"gdbmopen(%s): %s", vfname, gdbm_strerror(gdbm_errno));
  free(vfname);
}

void hcache_free(struct hcache *cache) {
  _hcache_validate_done(cache);
  if (cache->dbf) gdbm_close(cache->dbf);
  free(cache->path);
  free(cache);
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
  if (datum.dptr == NULL) return;
  if (gdbm_store(cache->validated, key, datum, GDBM_INSERT) == -1)
    fatal(gdbm_errno,"gdbm_store %s", gdbm_strerror(gdbm_errno));
  //ckpt(0);
  free(datum.dptr);
  free(key.dptr);
}
void hcache_put(struct hcache *cache, struct stat *st,char *hash,int len) {
  datum key, datum;
  _hcache_validate_done(cache);
  key = _hcache_genkey(cache,st);
  datum.dptr = hash;
  datum.dsize = len;
  if (gdbm_store(cache->dbf,key,datum,GDBM_REPLACE) == -1)
    fatal(gdbm_errno,"gdbm_store %s", gdbm_strerror(gdbm_errno));
  free(key.dptr);
}

int hcache_get(struct hcache *cache, struct stat *st,char **hash) {
  datum key, datum;
  _hcache_validate_done(cache);
  key = _hcache_genkey(cache,st);
  datum = gdbm_fetch(cache->dbf, key);
  free(key.dptr);
  if (hash) *hash = datum.dptr;
  return datum.dptr == NULL ? 0 : 1;
}

void hcache_del(struct hcache *cache, struct stat *st) {
  datum key;
  _hcache_validate_done(cache);
  key = _hcache_genkey(cache,st);
  gdbm_delete(cache->dbf,key);
  free(key.dptr);
}
