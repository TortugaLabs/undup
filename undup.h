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
#ifndef _UNDUP_H
#define _UNDUP_H
#include <stdio.h>

struct undup_opts {
  int dryrun;
  int usecache;
  int mstats;
  int cstats;
  int verbose;
  int scanonly;
};
extern struct undup_opts gopts;

int undup_main(int argc,char **argv);
#define vmsg(...) if (gopts.verbose) fprintf(stderr,__VA_ARGS__)
#endif
