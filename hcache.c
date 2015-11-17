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
  time_t mtime;
  off_t size;
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
  key.dptr = packt(&key.dsize,cache->ktempl,
		   st->st_ino,
		   st->st_mode & ~S_IFMT,
		   st->st_uid,
		   st->st_gid,
		   st->st_size,
		   st->st_mtime,
		   cache->type);
  //ckpt(0);
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

struct hcache *hcache_new(const char *cachefile,int type) {
  struct hcache *cache = (struct hcache *)mymalloc(sizeof(struct hcache));
  memset(cache,0,sizeof(struct hcache));
  cache->path = mystrdup(cachefile);
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
void hcache_put(struct hcache *cache, struct stat *st,char *part,char *full,int len) {
  datum key, datum;
  _hcache_validate_done(cache);
  key = _hcache_genkey(cache,st);
  datum.dptr = mymalloc(datum.dsize = 1+len*2);
  memset(datum.dptr,0,datum.dsize);
  datum.dptr[0] = (part ? HCACHE_PART : HCACHE_NONE) | (full ? HCACHE_FULL : HCACHE_NONE);
  if (part) memcpy(datum.dptr+1,part,len);
  if (full) memcpy(datum.dptr+(1+len),full,len);
  if (gdbm_store(cache->dbf,key,datum,GDBM_REPLACE) == -1)
    fatal(gdbm_errno,"gdbm_store %s", gdbm_strerror(gdbm_errno));
  free(datum.dptr);
  free(key.dptr);
}

int hcache_get(struct hcache *cache, struct stat *st,char **part,char **full,int len) {
  int res;
  datum key, datum;
  _hcache_validate_done(cache);
  key = _hcache_genkey(cache,st);
  datum = gdbm_fetch(cache->dbf, key);
  free(key.dptr);

  if (datum.dptr == NULL) {
    if (part) *part = NULL;
    if (full) *full = NULL;
    return 0;
  }
  res = datum.dptr[0];
  if (part) {
    if ((res & HCACHE_PART) == HCACHE_PART) {
      *part = mymalloc(len);
      memcpy(*part,datum.dptr+1,len);
    } else {
      *part = NULL;
    }
  }
  if (full) {
    if ((res & HCACHE_FULL) == HCACHE_FULL) {
      *full = mymalloc(len);
      memcpy(*full,datum.dptr+1+len,len);
    } else {
      *full = NULL;
    }
  }
  return res;
}

void hcache_del(struct hcache *cache, struct stat *st) {
  datum key;
  _hcache_validate_done(cache);
  key = _hcache_genkey(cache,st);
  gdbm_delete(cache->dbf,key);
  free(key.dptr);
}
