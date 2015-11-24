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

static void cb(char *dir, char *file,struct stat *stdat,void *ext) {
  printf("%s - %s %d %d\n", dir,file, stdat ==NULL, ext == NULL);
}

TEST(fscanner_checked) {
  char *base, tpl[] = "tmpdirXXXXXXX";
  base = mkdtemp(tpl);
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

  struct fs_dat fsdat;
  struct cat_cb catcb = { .callback = cb, .ext = NULL };
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
}
TEST(fscanner_test1) {
  /* Generate a test file system */
  char b[32], *base;
  base = populate(b,10000);
  assertTrue(base);
  if (!base) return;
  rm_rf(base);
}
