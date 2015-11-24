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
#include "inodetab.h"
#include "utils.h"
#include <string.h>
#include <uthash.h>

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
struct inodetab *inodetab_free(struct inodetab *tab) {
  struct _inodedat *s, *tmp;
  int i;
  HASH_ITER(hh, tab->hash, s, tmp) {
    HASH_DEL(tab->hash, s);
    for (i=0;i < s->cnt ; i++) free(s->paths[i]);
    free(s);
  }
  free(tab);
  return NULL;
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
    s->paths[s->cnt++] = mystrdup(fpath);
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

int inodetab_count(struct inodetab *tab) {
  return HASH_COUNT(tab->hash);
}

#ifdef _DEBUG
void inodetab_dump(struct inodetab *tab) {
  struct _inodedat *s, *tmp;
  int i;
  fprintf(stdout,"INODETABLE: count: %d\n", HASH_COUNT(tab->hash));
  HASH_ITER(hh, tab->hash, s, tmp) {
    fprintf(stdout,"ino:%llx ts:%lld cnt:%d slots:%d\n",
	    (long long)s->inode, (long long)s->mtime, s->cnt, s->slots);
    for(i=0; i< s->cnt; i++) {
      fprintf(stdout,"    %d) %s\n",i, s->paths[i]);
    }
  }
}
#endif
