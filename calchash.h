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
#ifndef _CALCHASH_H
#define _CALCHASH_H
struct hash_ctx;

#define CH_MD2 'x'
#define CH_MD5 '5'
#define CH_SHA1 '1'
#define CH_SHA256 '2'

#ifndef CH_HASH_TYPE
#define CH_HASH_TYPE CH_MD5
#endif

struct IHash {
  int hash_type;
  int hash_len;
  struct hash_ctx *(*hash_new_fn)();
  void (*hash_update_fn)(struct hash_ctx *ctx,char *data,int len);
  void (*hash_free_fn)(struct hash_ctx *ctx,char *hash);
  const char *hash_name;
};
extern struct IHash calchash;

#define hash_type() (calchash.hash_type)
#define hash_len() (calchash.hash_len)
#define hash_name() (calchash.hash_name)
#define hash_new() calchash.hash_new_fn()
#define hash_update(ctx,dat,len) calchash.hash_update_fn(ctx,dat,len)
#define hash_free(ctx,h) calchash.hash_free_fn(ctx,h)
char *hash_file(const char *file);
void hash_set(int type);
void hash_set_by_name(const char *name);

#endif
