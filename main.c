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
#include "undup.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>

int hconvert_main() {
  exit(EXIT_FAILURE);
}


int main(int argc,char **argv) {
  if (argc == 0) fatal(EXIT_FAILURE,"Exec error");
  if (strcmp(argv[0],"hconvert") == 0) return hconvert_main(argc,argv);
  if (argc > 1 && strcmp(argv[1],"hconvert") == 0) {
    --argc, ++argv;
    return hconvert_main(argc,argv);
  }
  return undup_main(argc,argv);
}
