/*
 * Undup
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "fscanner.h"
#include "utils.h"
#include "lockfile.h"
#include "calchash.h"

char *trimslashes(char *in) {
  int len = strlen(in)-1;
  while (len > 0 && in[len] == '/') in[len--] = '\000';
  return in;
}


int main(int argc,char **argv) {
  char *cat = NULL, *root, *cache;
  int mstats = false;
  int use_cache = false;
  int dryrun = true;
  int opt;
  struct fscanner_dat fs;

  while ((opt = getopt(argc,argv,"emCc:l:")) != -1) {
    switch (opt) {
    case 'c':
      cat = mystrdup(optarg);
      break;
    case 'l':
      lockfile(optarg);
      break;
    case 'e':
      dryrun = false;
      break;
    case 'C':
      use_cache = true;
      break;
    case 'm':
      mstats = true;
      break;
    default: /* '?' */
      fprintf(stderr,"Usage: %s [options] dir\n", argv[0]);
      fputs("\t-c catalogue: Create a file catalogue\n",stderr);
      fputs("\t-l lockfile: Create a exclusive lock\n",stderr);
      fputs("\t-C: enable hash caching\n",stderr);
      fputs("\t-e: execute (disables dry-run mode)\n",stderr);
      fputs("\t-m: show malloc statistics\n",stderr);
      exit(EXIT_FAILURE);
    }
  }
  if (optind >= argc) fatal(EXIT_FAILURE,"Expected arguments after options");

  root = trimslashes(argv[optind]);
  cache = use_cache ? mystrcat(root,".hcf",NULL) : NULL;

  fscanner_init(&fs, root, cat, cache, hash_type(),dryrun);
  fscanner(&fs);
  dedup(&fs);

  printf("Blocks freed: %d\n", fs.blocks);
  printf("Files de-duped: %d\n", fs.files);

  fscanner_close(&fs);

  if (cache) free(cache);
  if (mstats) malloc_stats();
  return 0;
}
