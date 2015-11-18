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

extern char version[];
struct opts_t gopts;

char *trimslashes(char *in) {
  int len = strlen(in)-1;
  while (len > 0 && in[len] == '/') in[len--] = '\000';
  return in;
}

int main(int argc,char **argv) {
  char *root;
  int opt;
  struct fscanner_dat fs;

  gopts_init(&gopts);

  while ((opt = getopt(argc,argv,"h?Vqv5SemCsc:l:")) != -1) {
    switch (opt) {
    case 'c':
      gopts.catfp = fopen(optarg,"w");
      if (!gopts.catfp) perror(optarg);
      break;
    case 'l':
      lockfile(optarg);
      break;
    case 'e':
      gopts.dryrun = false;
      break;
    case 'C':
      gopts.usecache = false;
      break;
    case 'm':
      gopts.mstats = true;
      break;
    case '5':
      hash_set(MD5);
      break;
    case 'S':
      hash_set(SHA256);
      break;
    case 's':
      gopts.scanonly = true;
      break;
    case 'v':
      gopts.verbose = true;
      break;
    case 'q':
      gopts.verbose = false;
      break;
    case 'V':
      printf("undup v%s\n",version);
      exit(0);
    case 'h':
    case '?':
    default: /* '?' */
      fprintf(stderr,"Usage: %s [options] dir\n", argv[0]);
      fputs("\t-c catalogue: create a file catalogue\n",stderr);
      fputs("\t-l lockfile: create a exclusive lock\n",stderr);
      fputs("\t-C: disable hash caching\n",stderr);
      fputs("\t-e: execute (disables dry-run mode)\n",stderr);
      fputs("\t-s: scan only\n",stderr);
      fputs("\t-5: use MD5 hashes\n",stderr);
      fputs("\t-S: use SHA256 hashes\n",stderr);
      fputs("\t-q: supress additional info\n",stderr);
      fputs("\t-v: show additional info\n",stderr);
      fputs("\t-V: version info\n",stderr);
      fputs("\t-h|?: this help message\n",stderr);
      exit(EXIT_FAILURE);
    }
  }
  if (optind >= argc) fatal(EXIT_FAILURE,"Expected arguments after options");

  vmsg("Using hash: %s\n", hash_name());

  root = trimslashes(argv[optind]);
  fscanner_init(&fs, root);
  fscanner(&fs);
  //inodetab_dump(fs.itab);
  //duptab_dump(fs.dtab);
  if (gopts.scanonly) {
    if (gopts.catfp) fclose(gopts.catfp);
    if (gopts.mstats) malloc_stats();
    fscanner_close(&fs);
    exit(0);
  }
  dedup(&fs);

  printf("Blocks freed: %d\n", fs.blocks);
  printf("Files de-duped: %d\n", fs.files);

  fscanner_close(&fs);

  if (gopts.catfp) fclose(gopts.catfp);
  if (gopts.mstats) malloc_stats();
  return 0;
}
