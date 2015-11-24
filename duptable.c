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
#include "duptable.h"
#include "utils.h"
#include <uthash.h>
#include <string.h>

#define SLOTQ 32

struct _dupkey {
  gid_t gid;
  uid_t uid;
  off_t size;
  mode_t mode;
  char cksum[1];
};

struct _dupentry {
  struct _dupkey *key;
  UT_hash_handle hh;
  int cnt;
  int slots;
  ino_t inodes[1];
};

struct duptab {
  struct _dupentry *hash;
  struct _dupentry *iter;
};
struct duptab *duptab_new() {
  struct duptab *tab = mymalloc(sizeof(struct duptab));
  tab->hash = NULL;
  tab->iter = NULL;
  return tab;
}

int duptab_count(struct duptab *tab) {
  return HASH_COUNT(tab->hash);
}
void duptab_add(struct duptab *tab,struct stat *stdat,int hlen,void *hash) {
  unsigned ks;
  if (hash == NULL || hlen < 1) hlen = 1;
  struct _dupkey *nkey = mymalloc(ks=sizeof(struct _dupkey)+hlen);
  //ckmsg("\n\t%08lx - KEYALLOC(%s,%d)\n",(unsigned long)nkey,__FILE__,__LINE__);
  struct _dupentry *s, *tmp;
  memset(nkey,0,ks);
  nkey->gid = stdat->st_gid;
  nkey->uid = stdat->st_uid;
  nkey->size = stdat->st_size;
  nkey->mode = stdat->st_mode & ~S_IFMT;
  if (hash) memcpy(nkey->cksum, hash, hlen);

  HASH_FIND(hh, tab->hash, nkey, ks, s);
  if (s == NULL) {
    s = mymalloc(sizeof(struct _dupentry)+SLOTQ*sizeof(ino_t));
    s->key = nkey;
    s->cnt = 0;
    s->slots = SLOTQ;
    memset(s->inodes,0,SLOTQ*sizeof(ino_t));
    HASH_ADD_KEYPTR(hh, tab->hash, s->key, ks, s);
  } else {
    //ckpt();
    //ckmsg("\n\t%08lx - KEYFREE(%s,%d)\n",(unsigned long)nkey,__FILE__,__LINE__);
    free(nkey);
    // Check if there are free slots...
    if (s->cnt == s->slots) {
      // Resize node
      HASH_DEL(tab->hash, s);
      tmp = mymalloc(sizeof(struct _dupentry)+(SLOTQ+s->slots)*sizeof(ino_t));
      tmp->key = s->key;
      tmp->cnt = s->cnt;
      tmp->slots = s->slots + SLOTQ;
      memcpy(tmp->inodes,s->inodes,tmp->cnt*sizeof(ino_t));
      memset(tmp->inodes+s->cnt,0,SLOTQ*sizeof(ino_t));
      HASH_ADD_KEYPTR(hh, tab->hash, tmp->key, ks, tmp);
      //ckpt();
      free(s);
      s = tmp;
    }
  }
  s->inodes[s->cnt++] = stdat->st_ino;
  //if (s->cnt > 1) fprintf(stderr,"CC: %d\n",(int)stdat->st_ino);
}

static int duptab_sort_function(void *a,void *b) {
  struct _dupentry *aa = (struct _dupentry *)a;
  struct _dupentry *bb = (struct _dupentry *)b;
  if (aa->key->size < bb->key->size) return 1;
  if (aa->key->size > bb->key->size) return -1;
  return 0;
}

void duptab_sort(struct duptab *tab) {
  HASH_SORT(tab->hash, duptab_sort_function);
}

struct duptab *duptab_free(struct duptab *tab) {
  struct _dupentry *s, *tmp;
  //HASH_ITER(hh, tab->hash, s, tmp) {
  //ckmsg("XCHK: %08lx\n",(unsigned long)s->key);
  //}

  HASH_ITER(hh, tab->hash, s, tmp) {
    HASH_DEL(tab->hash, s);
    //ckpt();
    //ckmsg("\n\t%08lx - KEYFREE(%s,%d)\n",(unsigned long)s->key,__FILE__,__LINE__);
    free(s->key);
    //ckpt();
    free(s);
  }
  //ckpt();
  free(tab);
  return NULL;
}
static ino_t *_duptab_getiter(struct duptab *tab,int *cnt, struct stat *st) {
  if (tab->iter == NULL) {
    if (cnt) *cnt = 0;
    return NULL;
  }
  if (cnt) *cnt = tab->iter->cnt;
  if (st) {
    st->st_gid = tab->iter->key->gid;
    st->st_uid = tab->iter->key->uid;
    st->st_size = tab->iter->key->size;
    st->st_mode = tab->iter->key->mode;
  }
  return tab->iter->inodes;
}

ino_t *duptab_first(struct duptab *tab,int *cnt, struct stat *st) {
  tab->iter = tab->hash;
  return _duptab_getiter(tab,cnt,st);
}
ino_t *duptab_next(struct duptab *tab,int *cnt, struct stat *st) {
  if (tab->iter) tab->iter = tab->iter->hh.next;
  return _duptab_getiter(tab,cnt,st);
}

#ifdef _DEBUG
static char *hex(int keylen,char *dat) {
  static char buf[128];
  int cnt, i;
  cnt = keylen - sizeof(struct _dupkey);

  //ckptm("%s (%d:%lu,%d)\n",dat,keylen,sizeof(struct _dupkey),cnt);

  if (cnt<2) return "<none>";
  if (cnt > 60) cnt = 60;
  for (i = 0; i < cnt ; i++) {
    sprintf(buf+(i<<1),"%02x",dat[i]);
  }
  buf[i<<1] = 0;
  return buf;
}

void duptab_dump(struct duptab *tab) {
  struct _dupentry *s, *tmp;
  int i;
  fprintf(stdout,"DUPTABLE: count: %d\n", HASH_COUNT(tab->hash));
  HASH_ITER(hh, tab->hash, s, tmp) {
    fprintf(stdout,"-id(u:%d,g:%d) sz:%lld m:%03o %s (%d/%d)\n",
	    s->key->uid, s->key->gid, (long long)s->key->size, s->key->mode,
	    hex(s->hh.keylen,s->key->cksum),
	    s->cnt, s->slots);
    for(i=0; i< s->cnt; i++) {
      fprintf(stdout,"    %d) %llx\n",i, (long long)s->inodes[i]);
    }
  }
}
#endif
