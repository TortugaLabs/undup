#include "inodetab.h"
#include "utils.h"
#include <string.h>

//
// Implemented using UTHASH
//
struct _inodedat {
  ino_t inode;
  UT_hash_handle hh;
  time_t mtime;
  int cnt;
  int slots;
  char *paths[1];
};
struct inodetab {
  struct _inodedat *hash;
};

struct inodetab *inodetab_new() {
  struct inodetab *tab = mymalloc(sizeof(struct inodetab));
  tab->hash = NULL;
  return tab;
}
void inodetab_free(struct inodetab *tab) {
  struct _inodedat *s, *tmp;
  int i;
  HASH_ITER(hh, tab->hash, s, tmp) {
    HASH_DEL(tab->hash, s);
    for (i=0;i < s->cnt ; i++) free(s->paths[i]);
    free(s);
  }
  free(tab);
}
int inodetab_add(struct inodetab *tab,ino_t ino,char *fpath,int nlnks,time_t mtime) {
  struct _inodedat *s;
  HASH_FIND_INT(tab->hash, &ino, s);
  if (s == NULL) {
    s = (struct _inodedat *)mymalloc(sizeof(struct _inodedat)+nlnks*sizeof(char *));
    s->inode = ino;
    s->mtime = mtime;
    s->cnt = 0;
    s->slots = nlnks;
    memset(s->paths,0,sizeof(char *)*(nlnks+1));
    HASH_ADD_INT(tab->hash, inode, s);
  }
  if (s->cnt < s->slots) {
    s->paths[s->cnt++] = fpath;
  } else {
    fatal(EXIT_FAILURE,"%s: links exceeded slots (%d)", fpath, s->slots);
  }
  //fprintf(stderr,"ADD: %s (%d - %lx)\n",fpath, s->cnt, (unsigned long)s);
  return s->cnt;
}

char **inodetab_get(struct inodetab *tab,ino_t ino,time_t *mtime) {
  struct _inodedat *s;
  HASH_FIND_INT(tab->hash, &ino, s);
  if (s == NULL) return NULL;
  if (mtime) *mtime = s->mtime;
  return s->paths;
}
