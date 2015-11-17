#include "duptable.h"
#include "utils.h"
#include <string.h>

const int SLOTQ = 32;

struct _dupkey {
  gid_t gid;
  uid_t uid;
  off_t size;
  mode_t mode;
  int sflag;
  int lflag;
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
void duptab_add(struct duptab *tab,struct stat *stdat,int hlen,void *shash,void *lhash) {
  int ks;
  struct _dupkey *nkey = mymalloc(ks=sizeof(struct _dupkey)+hlen*2);
  struct _dupentry *s, *tmp;
  memset(nkey,0,ks);
  nkey->gid = stdat->st_gid;
  nkey->uid = stdat->st_uid;
  nkey->size = stdat->st_size;
  nkey->mode = stdat->st_mode & ~S_IFMT;
  nkey->sflag = hlen > 0 && shash != NULL ? 1 : 0;
  nkey->lflag = hlen > 0 && lhash != NULL ? 1 : 0;
  if (shash) memcpy(nkey->cksum, shash, hlen);
  if (lhash) memcpy(nkey->cksum + hlen, lhash, hlen);

  HASH_FIND(hh, tab->hash, nkey, ks, s);
  if (s == NULL) {
    s = mymalloc(sizeof(struct _dupentry)+SLOTQ*sizeof(ino_t));
    s->key = nkey;
    s->cnt = 0;
    s->slots = SLOTQ;
    memset(s->inodes,0,SLOTQ*sizeof(ino_t));
    HASH_ADD_KEYPTR(hh, tab->hash, s->key, ks, s);
  } else {
    free(nkey);
    // Check if there are free slots...
    if (s->cnt == s->slots) {
      // Resize node
      tmp = mymalloc(sizeof(struct _dupentry)+(SLOTQ+s->slots)*sizeof(ino_t));
      tmp->key = s->key;
      tmp->cnt = s->cnt;
      tmp->slots = s->slots + SLOTQ;
      memcpy(tmp->inodes,s->inodes,tmp->cnt*sizeof(ino_t));
      memset(tmp->inodes+s->cnt,0,SLOTQ*sizeof(ino_t));
      HASH_REPLACE(hh, tab->hash, key, ks, tmp, s);
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

void duptab_free(struct duptab *tab) {
  struct _dupentry *s, *tmp;
  HASH_ITER(hh, tab->hash, s, tmp) {
    HASH_DEL(tab->hash, s);
    free(s->key);
    free(s);
  }
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
#ifdef XDEBUG
char *hex(char *dat,int cnt) {
  static char buf[128];
  int i;
  for (i = 0; i < cnt ; i++) {
    sprintf(buf+(i<<1),"%02x",dat[i]);
  }
  buf[i<<1] = 0;
  return buf;
}

void duptab_dump(struct duptab *tab) {
  extern int hash_len();
  struct _dupentry *s, *tmp;
  int i;
  fprintf(stderr,"DUPTABLE(%lx): count: %d\n",
	  (long)tab, HASH_COUNT(tab->hash));
  HASH_ITER(hh, tab->hash, s, tmp) {
    fprintf(stderr,"-id(u:%d,g:%d) sz:%lld m:%03o %s %s (%d/%d)\n",
	    s->key->uid, s->key->gid, (long long)s->key->size, s->key->mode,
	    s->key->sflag ? hex(s->key->cksum+1,hash_len()) : "<none>",
	    s->key->lflag ? hex(s->key->cksum+(1+hash_len()),hash_len()) : "<none>",
	    s->cnt, s->slots);
    for(i=0; i< s->cnt; i++) {
      fprintf(stderr,"    %d) %llx\n",i, (long long)s->inodes[i]);
    }
  }
}
#endif

