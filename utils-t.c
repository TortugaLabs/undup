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
#include "utils.h"
#include <stdio.h>
#include "test.h"

TEST(utils_printhex) {
  printhex(stdout,"\xca\xfe\x15\xba\xd1",5,0);
  putc('\n',stdout);
  fflush(stdout);
}

TEST(mymalloc1) {
  void *p, *q;
  p = _mymalloc(8192,__FILE__,__LINE__);
  assertTrue(p);
  if (p) free(p);

  if (forktest(ENOMEM)) {
    q = _mymalloc(-1,__FILE__,__LINE__); if (q) free(q);
    exit(0);
  }
}

TEST(strdup1) {
  const char msg[] = "message";
  char *p = mystrdup(msg);
  assertFalse(strcmp(msg,p));
  assertNotEquals(msg,p);
  free(p);
}

TEST(mystrcat1) {
  const char spc[] = " ";
  const char target[] ="the quick brown fox jumped over the lazy dog";
  char *p;
  p = mystrcat("the",spc,"quick",spc,"brown",spc,"fox",spc,"jumped",spc,
	       "over",spc,"the",spc,"lazy",spc,"dog",NULL);
  assertTrue(p);
  assertFalse(strcmp(p,target));
  free(p);
}

TEST(lockfile1) {
  char p[] = "lckXXXXXX";
  if (mkstemp(p) == -1) {
    assertFalse("mkstemp failed");
    return;
  }
  lockfile(p);
  unlink(p);
}
