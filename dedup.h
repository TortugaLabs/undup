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
#ifndef _DEDUP_H
#define _DEDUP_H
#include "duptable.h"
#include "fscanner.h"
#include <sys/types.h>
#include <sys/stat.h>

struct dedup_cb {
  void (*do_dedup)(struct fs_dat *fs,ino_t *inos,int cnt,struct stat *stp,void *ext);
  void *ext;
};
struct duptab *dedup_cluster(struct duptab *in);
void dedup_pass(struct fs_dat *fs,struct dedup_cb *cb);
#endif
