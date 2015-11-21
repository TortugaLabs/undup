#ifndef _TEST_H
#define _TEST_H
#include <sys/types.h>
#include <sys/stat.h>

int forktest(int xcode);
char *populate(char *base, int max);
void rm_rf(const char *dir);
void mkfile(const char *dir,const char *fn,const char *data);
void mklink(const char *dir,const char *old,const char *new);
ino_t getino(const char *dir, const char *file);
#endif
