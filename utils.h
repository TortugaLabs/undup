#ifndef _UTILS_H
/*
 * Generic utilities
 */
#define _UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#define oom() _errorexit(__FILE__,__LINE__,__func__,NULL)
#define uthash_fatal(msg) _fatal(EXIT_FAILURE,__FILE__,__LINE__, "%s", msg)
#include <utarray.h>
#include <uthash.h>

#ifdef PROD
#define trace(f,l) ((void)0)
#else
#define trace(f,l) fprintf(stderr,"\"%s\",%d: ",f,l)
#endif

static inline void _fatal(int errcode,const char *file,int line,const char *fmt,...) {
  trace(file,line);
  va_list ap;
  va_start(ap,fmt);vfprintf(stderr,fmt,ap);va_end(ap);
  putc('\n',stderr);
  exit(errcode);
}
static inline void _errorexit(const char *file,int line, const char *func, const char *fmt,...) {
  trace(file,line);
  if (fmt) {
    va_list ap;
    va_start(ap,fmt);vfprintf(stderr,fmt,ap);va_end(ap);
    fputs(": ",stderr);
    perror(NULL);
  } else {
    perror(func);
  }
  exit(errno);
}

static inline void *_mymalloc(size_t sz,const char *file,int line) {
  void *p = malloc(sz);
  if (!p) _fatal(ENOMEM,file,line,NULL);
  return p;
}
static inline char *_mystrdup(const char *str,const char *file,int line) {
  char *p = strdup(str);
  if (!p) _fatal(ENOMEM,file,line,NULL);
  return p;
}

static inline char *_mystrcat(const char *file,int line,const char *str,...) {
  va_list ap;
  const char *p;
  char *ptr, *d;
  size_t len = 0;
  va_start(ap,str);
  for(p = str; p != NULL; p = va_arg(ap,const char *)) len += strlen(p);
  va_end(ap);
  ptr = (char *)_mymalloc(len+1,file,line);
  va_start(ap,str);
  for (p = str, d = ptr; p != NULL; p = va_arg(ap,const char*)) {
    len = strlen(p);
    if (!len) continue;
    memcpy(d,p,len);
    d += len;
  }
  *d = '\0';
  return ptr;
}

#define mymalloc(sz) _mymalloc(sz,__FILE__,__LINE__)
#define mystrdup(str) _mystrdup(str,__FILE__,__LINE__)
#define mystrcat(str,...) _mystrcat(__FILE__,__LINE__,str,## __VA_ARGS__)
#define fatal(errcode,fmt,...) \
  _fatal(errcode,__FILE__,__LINE__,fmt , ## __VA_ARGS__)
#define errorexit(fmt,...) \
  _errorexit(__FILE__,__LINE__,__func__,fmt , ## __VA_ARGS__)
#endif
