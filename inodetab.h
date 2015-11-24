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
#ifndef _INODETAB_H
#define _INODETAB_H
#include <sys/types.h>

struct inodetab;
struct inodetab *inodetab_new();
struct inodetab *inodetab_free(struct inodetab *tab);
int inodetab_add(struct inodetab *tab,ino_t ino,char *fpath,int nlnks,time_t mtime);
char **inodetab_get(struct inodetab *tab,ino_t ino,time_t *mtime);
int inodetab_count(struct inodetab *tab);

#ifdef _DEBUG
void inodetab_dump(struct inodetab *tab);
#else
#define inodetab_dump(tab) ((void)0)
#endif
#endif
