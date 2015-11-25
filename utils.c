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
#include "utils.h"
#include <stdarg.h>

#ifdef _DEBUG
#include <stdlib.h>
#include <unistd.h>

static int trace_unlink = true;
#endif

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
#ifdef _DEBUG
  char *env = getenv("MYSTRCAT_TRACE");
  if (env) {
    if (trace_unlink) {
      unlink(env);
      trace_unlink = false;
    }
    FILE *pp = fopen(env,"a");
    fprintf(pp,"(%s,%d): %08llx %s\n", file,line,(long long unsigned)ptr, ptr);
    fclose(pp);
  }
#endif
  return ptr;
}
