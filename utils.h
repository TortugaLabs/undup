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
#ifndef _UTILS_H
/*
 * Generic utilities
 */
#define _UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#ifdef _DEBUG
#define trace(f,l) fprintf(stderr,"\"%s\",%d: ",f,l)
#define ckptm(...) do {						\
    fprintf(stderr,"%s,%d(%s): ",__FILE__,__LINE__,__func__);	\
    fprintf(stderr, __VA_ARGS__);				\
    fflush(stderr);						\
  } while(0)
#define ckpt() do {						\
    fprintf(stderr,"%s,%d(%s)\n",__FILE__,__LINE__,__func__);	\
    fflush(stderr);						\
  } while(0)
#else
#define trace(f,l) ((void)f,(void)l)
#define ckptm(...) ((void)0)
#define ckpt() ((void)0)
#endif

// These are needed for uthash and friends
#undef oom
#undef uthash_fatal
#define oom() errorexit()
#define uthash_fatal(msg) fatal(ENOMEM,"%s\n",msg)

#define printhex(fptr,data,max,len)	do {			\
    int i, cnt = (max), lnlen = (len);				\
    FILE *fp = (fptr);						\
    char *bytes = (char *)data;					\
    for (i=0;i<cnt;i++) {					\
      if (i>0 && lnlen > 0 && (i%lnlen) == 0) fputc('\n',fp);	\
      fprintf(fp,"%02x",bytes[i]&0xff);				\
    }								\
    if (lnlen>0) fputc('\n',fp);				\
  } while(0);

static inline void *_mymalloc(size_t sz,const char *file,int line) {
  void *p = malloc(sz);
  if (!p){
    trace(file,line);
    perror(__func__);
    exit(ENOMEM);
  }
  return p;
}
#ifdef _DEBUG
#define mymalloc(sz) malloc(sz)
#else
#define mymalloc(sz) _mymalloc(sz,__FILE__,__LINE__)
#endif
static inline char *_mystrdup(const char *str,const char *file,int line) {
  char *p = strdup(str);
  if (!p) {
    trace(file,line);
    perror(__func__);
    exit(ENOMEM);
  }
  return p;
}
#ifdef _DEBUG
#define mystrdup(str) strdup(str)
#else
#define mystrdup(str) _mystrdup(str,__FILE__,__LINE__)
#endif

#define fatal(errcode,...)	do {		\
  trace(__FILE__,__LINE__);			\
  fprintf(stderr,__VA_ARGS__);			\
  putc('\n',stderr);				\
  exit(errcode);				\
  }while(0)
#define errormsg(...)	do {			\
  trace(__FILE__,__LINE__);			\
  fprintf(stderr,__VA_ARGS__);			\
  fputs(": ",stderr);				\
  perror(NULL);					\
  exit(errno);					\
  }while(0)
#define errorexit() do {			\
  trace(__FILE__,__LINE__);			\
  perror(__func__);				\
  exit(errno);					\
  }while(0)

char *_mystrcat(const char *file,int line,const char *str,...);
#define mystrcat(...) _mystrcat(__FILE__,__LINE__, __VA_ARGS__,NULL)

#define lockfile(path)	do {					\
  const char *fp;						\
  fp = (path);							\
  int id = (int)getpid();					\
  int fd = open(fp,O_RDWR|O_CREAT,0666);			\
  if (fd == -1) errormsg("open(%s)",fp);			\
  write(fd,(void *)&id,sizeof(id));				\
  if (flock(fd,LOCK_EX) == -1) errormsg("flock(%s)",fp);	\
  } while(0)

#define trimslashes(str)	do {			\
    char *in = (str);					\
    int len = strlen(in)-1;				\
    while(len>0 && in[len] == '/') in[len--] = '\0';	\
  } while(0)
#endif
