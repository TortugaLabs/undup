/*
 *    This file is part of undup
 *    Copyright (C) 2018, Alejandro Liu
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
#ifndef _EXCLUDE_H
#define _EXCLUDE_H
#include <sys/types.h>
#include <sys/stat.h>

#define EXCLUDES_EXCLUDE	(1<<0)
#define EXCLUDES_INCLUDE	(1<<1)
#define EXCLUDES_FULLPATH	(1<<2)
#define EXCLUDES_FNMATCH	(1<<3)
#define EXCLUDES_MATCHDIR	(1<<4)

struct excludes_t {
  char *pattern;
  int flags;
  struct excludes_t *prev, *next;
};

struct excludes_t *excludes_add(struct excludes_t *tab, char *inp, int flags);
void excludes_free(struct excludes_t *tab);
int excludes_check(char *dir, char *file,struct stat *stdat,struct excludes_t *tab);
#endif
