#include <mcheck.h>
#include <cu.h>
#include <cu.c>

#include "test.h"
#include <stdlib.h>
#include <stdio.h>
#include "cu-t.h"
#include <time.h>
#include <unistd.h>
#include "utils.h"
#include <fcntl.h>

#define CLUSTER 100
enum ftypes_t { S_FILE, S_DIR, S_END };

static enum ftypes_t selectors[] = {
  S_FILE,
  S_FILE,
  S_FILE,
  S_FILE,
  S_FILE,
  S_DIR,
  S_END,
  S_END,
  S_END,
};

static void _populate(char *base, int *cnt, int max) {
  char buf[64], *p;
  int fd;
  while (*cnt < max) {
    int i = rand() % (sizeof(selectors)/sizeof(selectors[0]));
    switch (selectors[i]) {
    case S_FILE:
      /* Create a file */
      snprintf(buf,sizeof buf,"%x",rand());
      p = mystrcat(base,"/",buf);
      fd = open(p,O_WRONLY|O_CREAT|O_EXCL,0666);
      free(p);
      if (fd == -1) continue;
      snprintf(buf,sizeof buf,"%d", rand() % CLUSTER);
      write(fd,buf,strlen(buf));
      close(fd);
      ++(*cnt);
      break;
    case S_DIR:
      /* Create a directory */
      snprintf(buf,sizeof buf,"%x",rand());
      p = mystrcat(base,"/",buf);
      fd = mkdir(p,0777);
      if (fd == 0) {
	++(*cnt);
	_populate(p, cnt, max);
      }
      free(p);
      break;
    case S_END:
      return;
    }
  }
}

char *populate(char *base, int max) {
  int cnt = 0;
  strcpy(base,"tmpdirXXXXXXX");
  base = mkdtemp(base);
  if (base == NULL) return NULL;
  while (cnt < max) _populate(base,&cnt,max);
  return base;
}

void rm_rf(const char *dir) {
  char buf[4096];
  snprintf(buf,sizeof buf,"rm -rf \"%s\"", dir);
  system(buf);
}

void mkfile(const char *dir,const char *fn,const char *data) {
  char *p = mystrcat(dir,"/",fn);
  int fd = open(p,O_WRONLY|O_CREAT|O_EXCL,0666);
  free(p);
  write(fd,data,strlen(data));
  close(fd);
}
void mklink(const char *dir,const char *old,const char *new) {
  char *dold = mystrcat(dir,"/",old);
  char *dnew = mystrcat(dir,"/",new);
  link(dold,dnew);
  free(dnew);
  free(dold);
}
ino_t getino(const char *dir, const char *file) {
  char *p =mystrcat(dir,"/",file);
  struct stat stb;
  stb.st_ino = 0;
  lstat(p,&stb);
  free(p);
  return stb.st_ino;
}


int forktest(int xcode) {
  fflush(stdout);
  fflush(stderr);

  pid_t child = fork();
  assertNotEquals(child,-1);
  if (child == 0) return 1;
  if (child && child != -1) {
    int status  = 0;
    pid_t waited;
    waited = waitpid(child,&status,0);
    assertEquals(child,waited);
    assertTrue(WIFEXITED(status));
    assertEquals(WEXITSTATUS(status), xcode);
  }
  return 0;
}



int main(int argc, char **argv) {
  char *env;
  mtrace();
  if ((env = getenv("CU_SEED")) != NULL)
    srand(atoi(env));
  else {
    int seed = ((((unsigned)getpid())*((unsigned)time(NULL)))^((unsigned)time(NULL)))+((unsigned)clock());
    printf("repeat run with CU_SEED=%d\n",seed);
    srand(seed);
  }
  // Set up directory where are stored files with outputs from test
  // suites
  CU_SET_OUT_PREFIX("regressions/");

  // Run all test suites
  CU_RUN(argc, argv);

  muntrace();
  return 0;
}
