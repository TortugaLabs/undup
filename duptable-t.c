#include <cu.h>
#include "duptable.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define CLUSTER 50
#define MAX_ITEMS 1000000

static struct stat *mkstat(struct stat *stp,ino_t ino,uid_t uid,gid_t gid,off_t size,mode_t mode) {
  memset(stp,0,sizeof(struct stat));
  stp->st_ino = ino;
  stp->st_uid = uid;
  stp->st_gid = gid;
  stp->st_size = size;
  stp->st_mode = mode;
  return stp;
}

TEST(duptable_checked) {
  struct duptab *dt;
  struct stat stb;
  int cnt;
  ino_t *inos;
  struct mallinfo m1, m2;

  m1 = mallinfo();

  dt = duptab_new();
  assertTrue(dt);

  cnt = 1;
  inos = duptab_first(dt,&cnt,&stb);
  assertFalse(inos);
  assertFalse(cnt);
  inos = duptab_first(dt,NULL,NULL);
  assertFalse(inos);

  duptab_add(dt,
	     mkstat(&stb,/*ino*/1,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0777),
	     0,NULL);
  assertEquals(duptab_count(dt),1);
  duptab_add(dt,
	     mkstat(&stb,/*ino*/2,/*uid*/0,/*gid*/0,/*size*/10,/*mode*/0777),
	     0,NULL);
  assertEquals(duptab_count(dt),2);
  duptab_add(dt,
	     mkstat(&stb,/*ino*/3,/*uid*/0,/*gid*/0,/*size*/25,/*mode*/0777),
	     0,NULL);
  assertEquals(duptab_count(dt),3);

  duptab_add(dt,
	     mkstat(&stb,/*ino*/4,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0777),
	     0,NULL);
  assertEquals(duptab_count(dt),3);

  duptab_add(dt,
	     mkstat(&stb,/*ino*/5,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0777),
	     0,NULL);
  assertEquals(duptab_count(dt),3);

  duptab_add(dt,
	     mkstat(&stb,/*ino*/6,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0644),
	     0,NULL);
  assertEquals(duptab_count(dt),4);

  duptab_add(dt,
	     mkstat(&stb,/*ino*/7,/*uid*/2,/*gid*/2,/*size*/100,/*mode*/0644),
	     0,NULL);
  assertEquals(duptab_count(dt),5);
  duptab_add(dt,
	     mkstat(&stb,/*ino*/8,/*uid*/2,/*gid*/2,/*size*/100,/*mode*/0644),
	     0,NULL);
  assertEquals(duptab_count(dt),5);

  inos = duptab_first(dt,&cnt,&stb);
  assertEquals(*inos,1);
  inos = duptab_next(dt,&cnt,&stb);
  assertEquals(*inos,2);

  duptab_sort(dt);
  inos = duptab_first(dt,&cnt,&stb);
  assertEquals(stb.st_size,100);
  inos = duptab_next(dt,&cnt,&stb);
  assertEquals(stb.st_size,100);

  duptab_free(dt);

  dt = duptab_new();
  char *hashes[] = {
    "lskjf;asf",
    "kll.xx;;s",
    "lskksmmsf"
  };
  duptab_add(dt,
	     mkstat(&stb,/*ino*/1,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0777),
	     8,hashes[0]);
  assertEquals(duptab_count(dt),1);
  duptab_add(dt,
	     mkstat(&stb,/*ino*/2,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0777),
	     8,hashes[1]);
  assertEquals(duptab_count(dt),2);
  duptab_add(dt,
	     mkstat(&stb,/*ino*/3,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0777),
	     8,hashes[2]);
  assertEquals(duptab_count(dt),3);
  duptab_add(dt,
	     mkstat(&stb,/*ino*/4,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0777),
	     8,hashes[0]);
  assertEquals(duptab_count(dt),3);
  duptab_add(dt,
	     mkstat(&stb,/*ino*/5,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0777),
	     8,hashes[0]);
  assertEquals(duptab_count(dt),3);
  duptab_add(dt,
	     mkstat(&stb,/*ino*/6,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0777),
	     8,hashes[1]);
  assertEquals(duptab_count(dt),3);

  inos = duptab_first(dt,&cnt,&stb);
  assertEquals(*inos,1);
  inos = duptab_next(dt,&cnt,&stb);
  assertEquals(*inos,2);

  duptab_free(dt);

  dt = duptab_new();
  for (int i=1;i<50;i++)
    duptab_add(dt,
	       mkstat(&stb,/*ino*/i,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0777),
	       0,NULL);
  duptab_free(dt);

  m2 = mallinfo();
  assertEquals(m1.uordblks,m2.uordblks);
}

TEST(duptable_output) {
  int max = 5;
  struct duptab *dt;
  struct stat stb;

  dt = duptab_new();
  for (int j=0;j < max; j++) {
    for (int i=0;i < max;i++) {
      duptab_add(dt,
		 mkstat(&stb,/*ino*/i+j*max,/*uid*/0,/*gid*/0,/*size*/i,/*mode*/0777),
		 0,NULL);
    }
  }
  duptab_dump(dt);
  duptab_free(dt);

  dt = duptab_new();
  for (int j=0;j < max; j++) {
    for (int i=0;i < max;i++) {
      char buf[128];
      snprintf(buf,sizeof(buf),"%08d",j);
      duptab_add(dt,
		 mkstat(&stb,/*ino*/i+j*max,/*uid*/0,/*gid*/0,/*size*/100,/*mode*/0777),
		 strlen(buf),buf);
    }
  }
  duptab_dump(dt);
  duptab_free(dt);

}

TEST(duptable_large) {
  struct duptab *dt;
  struct stat stb;
  struct mallinfo m1, m2;

  m1 = mallinfo();
  dt = duptab_new();
  for (int j=0;j < MAX_ITEMS; j++) {
    int size = rand() % CLUSTER;
    duptab_add(dt,
	       mkstat(&stb,/*ino*/j,/*uid*/0,/*gid*/0,/*size*/size,/*mode*/0777),
	       0,NULL);
  }
  duptab_sort(dt);
  duptab_free(dt);

  dt = duptab_new();
  for (int j=0;j < MAX_ITEMS; j++) {
    char buf[128];
    snprintf(buf,sizeof buf,"%x",rand() % CLUSTER);
    duptab_add(dt,
	       mkstat(&stb,/*ino*/j,/*uid*/0,/*gid*/0,/*size*/9,/*mode*/0777),
	       strlen(buf),buf);
  }
  duptab_sort(dt);
  duptab_free(dt);
  m2 = mallinfo();
  assertEquals(m1.uordblks,m2.uordblks);
}
