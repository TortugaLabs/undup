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
#include <cu.h>
#include "fscanner.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "inodetab.h"
#include "duptable.h"
#include "test.h"
#include <uthash.h>
#include <string.h>

struct fsout {
  int id;
  UT_hash_handle hh;
  char text[1];
};
struct cb_data {
  struct fsout *lines;
  int cid;
};
  

static int cb(char *dir, char *file,struct stat *stdat,void *ext) {
  struct cb_data *cbdata = (struct cb_data *)ext;
  int id = ++(cbdata->cid), k;
  struct fsout *ln = (struct fsout *)mymalloc(sizeof(struct fsout)+( k = strlen(dir)+strlen(file)+128));
  ln->id = id;
  snprintf(ln->text,k, "%s - %s %d %d", dir, file, stdat == NULL, ext == NULL);
  HASH_ADD_INT(cbdata->lines,id, ln);
  return 0;
}

static int line_sort(struct fsout *a,struct fsout *b) {
  return strcmp(a->text, b->text);
}

TEST(fscanner_checked) {
  char *base, tpl[] = "/tmp/tmpdirXXXXXXX";
  base = mkdtemp(tpl);
  //fprintf(stderr,"%s %s\n",base,tpl);
  assertTrue(base);
  if (base == NULL) return;
  mkfile(base,"msg1.txt","one");
  mkfile(base,"msg1a.txt","one");
  mkfile(base,"msg2.txt","two");
  mkfile(base,"msg3.txt","one1");
  mkfile(base,"msg3a.txt","one1");
  mklink(base,"msg3a.txt","msg3a.lnk");
  mkfile(base,"abc.txt","datafactor");
  mkfile(base,"abcd.txt","datafactors");

  struct cb_data cbdata = { .lines = NULL, .cid = 0 };
  struct fs_dat fsdat;
  struct cat_cb catcb = { .callback = cb, .ext = (void *)&cbdata };
  ino_t ino;
  fscanner_init(&fsdat, base, false);
  fscanner(&fsdat, &catcb);
  assertEquals(inodetab_count(fsdat.itab),7);
  ino = getino(base,"msg3a.lnk");
  assertTrue(inodetab_get(fsdat.itab, ino,NULL));
  assertFalse(inodetab_get(fsdat.itab, 0,NULL));
  //inodetab_dump(fsdat.itab);
  assertEquals(duptab_count(fsdat.dtab),4);
  //duptab_dump(fsdat.dtab);

  fscanner_close(&fsdat);
  rm_rf(base);
  
  // output stuff
  HASH_SORT(cbdata.lines, line_sort);
  struct fsout *p;
  for (p = cbdata.lines ; p != NULL ; p = (struct fsout *)(p->hh.next)) {
    puts(p->text);
  }
}
TEST(fscanner_test1) {
  /* Generate a test file system */
  char b[32], *base;
  base = populate(b,10000);
  assertTrue(base);
  if (!base) return;
  rm_rf(base);
}
