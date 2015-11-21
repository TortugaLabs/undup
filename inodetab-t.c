#include <cu.h>
#include "inodetab.h"
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

#define MAX_INODES 20000
#define MAX_LINKS 64

TEST(inodetab_basic) {
  int i;
  char buf[128];
  struct inodetab *tab;
  tab = inodetab_new();
  for (i=0; i < 10; i++) {
    snprintf(buf,sizeof(buf),"%d",i);
    inodetab_add(tab, i, buf, 1, i);
  }
  inodetab_dump(tab);
  inodetab_free(tab);
}

TEST(inodetab_limits) {
  /* add beyond nlnks */
  if (forktest(EXIT_FAILURE)) {
    struct inodetab *tab;
    tab = inodetab_new();
    inodetab_add(tab, 1, "one",1, 1);
    inodetab_add(tab, 1, "one+one",2, 1);
    inodetab_free(tab);
    exit(0);
  }
}


TEST(inodetab_checked) {
  struct inodetab *tab;
  time_t mtime;
  char **paths;

  tab = inodetab_new();
  assertFalse(inodetab_count(tab));
  inodetab_add(tab, 1, "one",2, 1);
  assertEquals(inodetab_count(tab),1);
  inodetab_add(tab, 1, "one+one",2, 1);
  assertEquals(inodetab_count(tab),1);
  inodetab_add(tab, 2, "two",1, 2);
  assertEquals(inodetab_count(tab),2);

  assertTrue(inodetab_get(tab,1,NULL));
  assertFalse(inodetab_get(tab,3,NULL));
  paths = inodetab_get(tab,2,&mtime);
  assertEquals(mtime,2);
  assertTrue(paths[0]);
  assertFalse(paths[1]);
  inodetab_free(tab);
}

TEST(inodetab_large) {
  char buf[128];
  struct inodetab *tab;

  tab = inodetab_new();

  for (int i=0; i < MAX_INODES; i++) {
    int nlnks = (i % MAX_LINKS) +1;
    for (int j = 0 ; j < nlnks ; j++) {
      snprintf(buf,sizeof(buf),"%d.%d",i,j);
      inodetab_add(tab, i, buf, nlnks, i);
    }
  }
  inodetab_free(tab);
}


TEST(inodetab_large_random) {
  char buf[128];
  struct inodetab *tab;

  tab = inodetab_new();
  int nums = 0;

  for (int i=0; i < MAX_INODES*10; i++) {
    int inode = rand();
    char **fpt = inodetab_get(tab,inode,NULL);
    if (fpt != NULL) {
      int cnt; // This entry has already enough links
      for (cnt=0;fpt[cnt];++cnt);
      if (cnt >= MAX_LINKS) continue;
    } else ++nums;
    snprintf(buf,sizeof(buf),"%d.%d",i,rand());
    inodetab_add(tab, inode, buf, MAX_LINKS, inode);
  }
  assertEquals(nums,inodetab_count(tab));
  inodetab_free(tab);
}
