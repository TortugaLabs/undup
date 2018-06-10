/*
 *    This file is part of undup
 *    Copyright (C) 2018, Alejandro Liu
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
#include "exclude.h"
#include <utlist.h>
#include "utils.h"
#include <fnmatch.h>
#include <string.h>

struct excludes_t *excludes_add(struct excludes_t *tab, char *inp, int flags) {
  struct excludes_t *new = (struct excludes_t *)mymalloc(sizeof(struct excludes_t));
  static const char p[] = "*?[]"; // Special pattern characters
  const char *q = p;
  while (*q) {
    if (strchr(inp,*(q++))) {
      flags |= EXCLUDES_FNMATCH;
      break;
    }
  }
  new->pattern = inp;
  new->flags = flags;
  DL_APPEND(tab, new);
  return tab;
}
void excludes_free(struct excludes_t *tab) {
  struct excludes_t *c,*tmp;
  DL_FOREACH_SAFE(tab,c,tmp) {
    DL_DELETE(tab,c);
    free(c);
  }
}
  
int excludes_check(char *dir, char *file,struct stat *stdat,struct excludes_t *tab) {
  struct excludes_t *pp;
  char *fp = NULL, *tmp;
  int r;

  DL_FOREACH(tab,pp) {
    if ((pp->flags & EXCLUDES_MATCHDIR) == EXCLUDES_MATCHDIR &&
	!S_ISDIR(stdat->st_mode)) continue;
    if ((pp->flags & EXCLUDES_FULLPATH) == EXCLUDES_FULLPATH && dir[0] != '\0') {
      if (fp == NULL) {
	int len;
	fp = mymalloc(len = strlen(dir)+strlen(file)+3);
	snprintf(fp,len-1,"%s/%s",dir,file);
      }
      tmp = fp;
    } else {
      tmp = file;
    }
    //printf("OK: tmp=%s (dir=%s file=%s pattern=%s)\n", tmp, dir, file, pp->pattern);
    if ((pp->flags & EXCLUDES_FNMATCH) == EXCLUDES_FNMATCH) {
      r = fnmatch(pp->pattern, tmp, 0);
    } else {
      r = strcmp(pp->pattern, tmp);
    }
    if (!r) {
      // Found!
      if (fp) free(fp);
      return ((pp->flags & EXCLUDES_EXCLUDE) == EXCLUDES_EXCLUDE);
    }
  }
  if (fp) free(fp);
  return 0;
}

