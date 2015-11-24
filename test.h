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
#ifndef _TEST_H
#define _TEST_H
#include <sys/types.h>
#include <sys/stat.h>

int forktest(int xcode);
char *populate(char *base, int max);
void rm_rf(const char *dir);
void mkfile(const char *dir,const char *fn,const char *data);
void mklink(const char *dir,const char *old,const char *new);
ino_t getino(const char *dir, const char *file);
#endif
