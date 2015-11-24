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
#ifndef _DUPTABLE_H
#define _DUPTABLE_H
#include <sys/types.h>
#include <sys/stat.h>

struct duptab;
struct duptab *duptab_new();
struct duptab *duptab_free(struct duptab *tab);
void duptab_add(struct duptab *tab,struct stat *stdat,int hlen,void *hash);
void duptab_sort(struct duptab *tab);
int duptab_count(struct duptab *tab);

ino_t *duptab_first(struct duptab *tab,int *cnt,struct stat *st);
ino_t *duptab_next(struct duptab *tab,int *cnt, struct stat *st);

#ifdef _DEBUG
void duptab_dump(struct duptab *tab);
#else
#define duptab_dump(tab) ((void)0)
#endif
#endif
