/*
 *    This file is part of undup
 *    Copyright (C) 2015, Alejandro Liu
 *
 *    undup is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    undup is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, see <http://www.gnu.org/licenses>
 */
#include "utils.h"
#include "hcache.h"
#include <gdbm.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

/*
 * Validating means we run through the filesystem and make sure
 * that hashes are still valid. i.e. an i-node still exists and it
 * has not been modified.
 *
 * This is automatic in a sense that if we open an "hcd", it will
 * not start validating, but will only do so on the first call to
 * validate.  If validate is not called, then we don't kick it off.
 * 
 * The first read (get) will stop the validation and make the validated
 * data active.
 * 
 * This means that on normal undup operation, we do the validation
 * and hashes are expired, before we start checking duplicates.
 * 
 * If we were only to look at the hcache file, this code will simply
 * open the "hcd" file, and (if no validation is called), you can
 * simply read the keys directly.
 * 
 */
#define HCM_READING	1
#define HCM_VALIDATING	2
#define HCM_RDWR	3

struct hcache {
  GDBM_FILE dbf;
  GDBM_FILE validated;
  char *path;
  int type;
  int hlen;
  int hits;
  int misses;
  int state;
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

static int is_dir(const char *name) {
  DIR *dp = opendir(name);
  if (dp != NULL) {
    closedir(dp);
    return 1;
  }
  return 0;
}

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
  //~ ckptm("i:%llx m:%03o s:%lld %lld %c\n",(long long)sk->inode,sk->mode,(long long)sk->size,(long long)sk->mtime, sk->cktype);
  return key;
}

const char *hcache_getpath(struct hcache *cache) {
  return cache->path;
}

void hcache_stats(struct hcache *cache,int *hits, int *misses){
  if(hits) *hits = cache->hits;
  if(misses) *misses = cache->misses;
}

struct hcache *hcache_new(const char *base,int type,int len) {
  struct hcache *cache = (struct hcache *)mymalloc(sizeof(struct hcache));
  char *cachefile = mystrcat(base, is_dir(base) ? "/.hcd" : ".hcd");

  memset(cache,0,sizeof(struct hcache));
  cache->path = cachefile;
  cache->dbf = gdbm_open(cachefile,0,GDBM_READER,0666,NULL);
  cache->type = type;
  cache->hlen = len;
  cache->state = HCM_READING;
  //~ ckpt();
  return cache;
}

static void _hcache_validate_done(struct hcache *cache,int iswr) {
  char *vfname;
  
  if (!cache->validated) {
    //~ ckptm("IsWR=%s state=%d\n",iswr ? "TRUE" : "FALSE", cache->state);
    if (!iswr && cache->state == HCM_RDWR) return;
    if (cache->dbf) gdbm_close(cache->dbf);
  } else {
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
  }
  cache->dbf = gdbm_open(cache->path,0, GDBM_WRCREAT, 0666,NULL);
  cache->state = HCM_RDWR;
}
static void _hcache_validate_init(struct hcache *cache) {
  char *vfname;
  if (cache->validated) return;
  cache->state = HCM_VALIDATING;
  vfname = mystrcat(cache->path,".validate");
  cache->validated = gdbm_open(vfname,0, GDBM_NEWDB, 0666,NULL);
  if (!cache->validated)
    fatal(gdbm_errno,"gdbmopen(%s): %s", vfname, gdbm_strerror(gdbm_errno));
  free(vfname);
}

struct hcache *hcache_free(struct hcache *cache) {
  _hcache_validate_done(cache,false);
  if (cache->dbf) gdbm_close(cache->dbf);
  free(cache->path);
  free(cache);
  return NULL;
}
void hcache_validate(struct hcache *cache, struct stat *st) {
  datum key, datum;
  _hcache_validate_init(cache);
  if (!cache->dbf) return;
  key = _hcache_genkey(cache,st);
  datum = gdbm_fetch(cache->dbf, key);
  if (datum.dptr == NULL) {
    free(key.dptr);
    return;
  }
  if (gdbm_store(cache->validated, key, datum, GDBM_INSERT) == -1)
    fatal(gdbm_errno,"gdbm_store %s", gdbm_strerror(gdbm_errno));
  free(datum.dptr);
  free(key.dptr);
}
void hcache_put(struct hcache *cache, struct stat *st,char *hash) {
  datum key, datum;
  _hcache_validate_done(cache,true);
  key = _hcache_genkey(cache,st);
  //printhex(stderr,key.dptr,key.dsize,0);

  datum.dptr = hash;
  datum.dsize = cache->hlen;
  //~ ckptm("hash=%s, len=%d (%llx)\n",hash,cache->hlen,((unsigned long long)cache->dbf));
  if (gdbm_store(cache->dbf,key,datum,GDBM_REPLACE) == -1)
    fatal(gdbm_errno,"gdbm_store %s", gdbm_strerror(gdbm_errno));
  free(key.dptr);
}

int hcache_get(struct hcache *cache, struct stat *st,char **hash) {
  datum key, datum;
  
  if (cache->dbf == NULL) return false;
  _hcache_validate_done(cache,false);
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
  _hcache_validate_done(cache,true);
  key = _hcache_genkey(cache,st);
  gdbm_delete(cache->dbf,key);
  free(key.dptr);
}
