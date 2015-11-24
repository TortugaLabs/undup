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
#ifndef _FSCANNER_H
#define _FSCANNER_H
#include "inodetab.h"
#include "duptable.h"
#include "hcache.h"
#include <sys/types.h>
#include <sys/stat.h>

struct fs_dat {
  struct inodetab *itab;
  struct duptab *dtab;
  struct hcache *cache;
  char *root;
};

struct cat_cb {
  void (*callback)(char *dir,char *file, struct stat *stdat,void *ext);
  void *ext;
};

void fscanner_init(struct fs_dat *fs,char *root,int usecache);
void fscanner(struct fs_dat *dat, struct cat_cb *cb);
void fscanner_close(struct fs_dat *scandat);


#endif
