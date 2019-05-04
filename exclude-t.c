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
#include "test.h"
#include "exclude.h"
#include <malloc.h>
#include <string.h>
#include <utlist.h>
#include <stdio.h>

//~ #include <stdlib.h>

TEST(exclude_leaktest) {
  char *pats[] = {
    "one",
    "two-three",
    "five size",
    "seven*eleven",
    "one??two",
    NULL,
  };
  char **ptr = pats;
  struct excludes_t *tab = NULL;
#ifdef __GLIBC__
  struct mallinfo m1, m2;

  m1 = mallinfo();
#endif

  while (*ptr) {
    tab = excludes_add(tab, *ptr, EXCLUDES_EXCLUDE);
    ++ptr;
  }
  excludes_free(tab);
#ifdef __GLIBC__
  m2 = mallinfo();
  assertEquals(m1.uordblks,m2.uordblks);
#endif
}

TEST(exclude_strcmp_or_fnmatch) {
  struct excludes_t *tab = NULL, *p;
  tab = excludes_add(tab, ".git", EXCLUDES_EXCLUDE | EXCLUDES_MATCHDIR);
  tab = excludes_add(tab, "*.srm", EXCLUDES_EXCLUDE);
  tab = excludes_add(tab, "something [a-b] more", EXCLUDES_INCLUDE);
  tab = excludes_add(tab, "*tmp????ckd", EXCLUDES_EXCLUDE| EXCLUDES_MATCHDIR);
  tab = excludes_add(tab, "*abcd????x/bckd?????", EXCLUDES_INCLUDE|EXCLUDES_FULLPATH);
  tab = excludes_add(tab, "abcdmonx", EXCLUDES_INCLUDE|EXCLUDES_FULLPATH);
  DL_FOREACH(tab,p) {
    printf("%02x %s\n", p->flags, p->pattern);
  }
  excludes_free(tab);
}

#define CKIF_SKIP_FIL(a,b)	assertTrue(excludes_check((a),(b),file,tab))
#define CKIF_INCL_FIL(a,b)	assertFalse(excludes_check((a),(b),file,tab))
#define CKIF_SKIP_DIR(a,b)	assertTrue(excludes_check((a),(b),dir,tab))
#define CKIF_INCL_DIR(a,b)	assertFalse(excludes_check((a),(b),dir,tab))


TEST(exclude_matches) {
#ifdef __GLIBC__
  struct mallinfo m1, m2;
#endif
  struct excludes_t *tab = NULL;
  static struct stat filebuf, dirbuf;
  struct stat *file = &filebuf, *dir = &dirbuf;
  memset(file,0,sizeof *file);
  memset(dir, 0, sizeof *dir);
  dirbuf.st_mode = S_IFDIR;

#ifdef __GLIBC__
  m1 = mallinfo();
#endif

  tab = excludes_add(tab, ".git", EXCLUDES_EXCLUDE | EXCLUDES_MATCHDIR);
  tab = excludes_add(tab, "*.srm", EXCLUDES_EXCLUDE);
  tab = excludes_add(tab, "admin", EXCLUDES_EXCLUDE | EXCLUDES_MATCHDIR | EXCLUDES_FULLPATH);
  tab = excludes_add(tab, "fort/nite", EXCLUDES_EXCLUDE | EXCLUDES_FULLPATH);
  tab = excludes_add(tab, "myfile.png", EXCLUDES_INCLUDE);
  tab = excludes_add(tab, "*.png", EXCLUDES_EXCLUDE);
  tab = excludes_add(tab, "xyz/abc/*", EXCLUDES_EXCLUDE | EXCLUDES_FULLPATH);

  CKIF_INCL_FIL("skdfk/jdf/lkdjf","kxhh");
  CKIF_INCL_DIR("skdfk/jdf/lkdjf","kxhh");

  CKIF_INCL_FIL("abc/cbd",".git");
  CKIF_SKIP_DIR("abc/cbd",".git");
  CKIF_SKIP_DIR("",".git");
  CKIF_INCL_FIL("",".git");
  CKIF_INCL_DIR("",".gitabc");
  CKIF_INCL_FIL("",".gitabc");

  CKIF_SKIP_FIL("","secret of mana.srm");
  CKIF_SKIP_FIL("asdfkjs/lkcb/ckjd","secret of mana.srm");
  
  CKIF_SKIP_DIR("","admin");
  CKIF_INCL_FIL("","admin");
  CKIF_INCL_FIL("abc/cbc","admin");
  CKIF_INCL_DIR("abc/cbc","admin");

  CKIF_SKIP_DIR("fort","nite");
  CKIF_SKIP_FIL("fort","nite");
  CKIF_INCL_DIR("is/fort","nite");
  CKIF_INCL_FIL("is/fort","nite");

  CKIF_INCL_FIL("abc/cb","myfile.png");
  CKIF_SKIP_FIL("xabc/cb","xmyfile.png");

  CKIF_SKIP_FIL("xyz/abc","ccd");
  CKIF_SKIP_FIL("xyz/abc","zzzdf");
  CKIF_SKIP_DIR("xyz/abc","sdkfjsdf");

  excludes_free(tab);
#ifdef __GLIBC__
  m2 = mallinfo();
  assertEquals(m1.uordblks,m2.uordblks);
#endif
}
