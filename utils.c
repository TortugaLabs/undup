#include "utils.h"
#include <stdarg.h>

char *_mystrcat(const char *file,int line,const char *str,...) {
  va_list ap;
  const char *p;
  char *ptr, *d;
  size_t len = 0;
  va_start(ap,str);
  for(p = str; p != NULL; p = va_arg(ap,const char *)) len += strlen(p);
  va_end(ap);
#ifdef _DEBUG
  ptr = (char *)malloc(len+1);
  ((void)file);
  ((void)line);
#else
  ptr = (char *)_mymalloc(len+1,file,line);
#endif
  va_start(ap,str);
  for (p = str, d = ptr; p != NULL; p = va_arg(ap,const char*)) {
    len = strlen(p);
    if (!len) continue;
    memcpy(d,p,len);
    d += len;
  }
  *d = '\0';
  /*
  FILE *pp = fopen("xout.txt","a");
  fprintf(pp,"(%s,%d): %08llx %s\n", file,line,(long long unsigned)ptr, ptr);
  fclose(pp);
  */
  return ptr;
}
