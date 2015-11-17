#ifndef _LOCK_H
#define _LOCK_H
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include "utils.h"

#define lockfile(fp)	do {					\
  int id = (int)getpid();					\
  int fd = open(fp,O_RDWR|O_CREAT,0666);			\
  if (fd == -1) errorexit("open(%s)",fp);			\
  write(fd,(void *)&id,sizeof(id));				\
  if (flock(fd,LOCK_EX) == -1) errorexit("flock(%s)",fp);	\
  } while(0)

#endif
