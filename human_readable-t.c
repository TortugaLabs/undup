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
#include <cu.h>
#include "human_readable.h"
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

//make_human_readable_str(unsigned long long val,
//				    unsigned long block_size,
//				    unsigned long display_unit);

TEST(human_readable_str_t1) {
  int i;
  unsigned long long v;
  for (i=60,v=1 ; --i ; v = v<<1) {
    printf("%llu %s\n",v,make_human_readable_str(v,0,0));
  }
  for (i=60,v=11 ; --i ; v = v<<1) {
    printf("%llu %s\n",v,make_human_readable_str(v,0,0));
  }
  for (i=50,v=11 ; --i ; v = v<<1) {
    printf("%llu %s\n",v,make_human_readable_str(v,1024,0));
  }
  fflush(stdout);
}
