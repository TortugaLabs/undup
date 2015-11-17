#include "utils.h"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fscanner.h"

static dev_t getfiledev(const char *f) {
  struct stat stbuf;
  if (stat(f,&stbuf) == -1) errorexit("lstat(%s)",f);
  return stbuf.st_dev;
}

static const char *filetype(int mode) {
  switch (mode & S_IFMT) {
  case S_IFSOCK: return "soc";
  case S_IFLNK: return "lnk";
  case S_IFREG: return "reg";
  case S_IFBLK: return "blk";
  case S_IFDIR: return "dir";
  case S_IFCHR: return "chr";
  case S_IFIFO: return "pip";
  }
  return "???";
}

void fscanner_init(struct fscanner_dat *dat, char *root, const char *cat,const char *cachefile,int type,int dryrun) {
  dat->root = root;
  dat->itab = inodetab_new();
  dat->dtab = duptab_new();
  dat->catfmt = DFT_CATFMT;
  if (cat) {
    dat->catfp = fopen(cat,"w");
    if (dat->catfp == NULL) errorexit("%s",cat);
  } else
    dat->catfp = NULL;
  if (cachefile) {
    dat->cache = hcache_new(cachefile,type);
  } else {
    dat->cache = NULL;
  }
  dat->blocks = 0;
  dat->files = 0;
  dat->dryrun = dryrun;
}

void fscanner_close(struct fscanner_dat *dat) {
  inodetab_free(dat->itab);
  duptab_free(dat->dtab);
  if (dat->catfp) fclose(dat->catfp);
  if (dat->cache) hcache_free(dat->cache);
}
void fscanner(struct fscanner_dat *dat) {
  const char *root = dat->root;
  UT_array *dirs;
  char *s = "";
  DIR *dh;
  struct dirent *dp;
  char *cdir, *dirpath, *fpath;
  struct stat stbuf;
  dev_t rootdev;

  rootdev = getfiledev(root);

  utarray_new(dirs, &ut_str_icd);
  utarray_push_back(dirs, &s);
  //ckpt(0);
  while (utarray_len(dirs) > 0) {
    cdir = mystrdup(*((char **)utarray_back(dirs)));
    utarray_pop_back(dirs);

    dirpath = mystrcat( root , cdir[0] ? "/" : "", cdir, NULL);
    dh = opendir(dirpath);
    if (dh == NULL) errorexit("opendir(%s)",dirpath);

    while ((dp = readdir(dh)) != NULL) {
      //ckpt(0);
      if (dp->d_name[0] == '.'
	&& (dp->d_name[1] == '\0'
		|| (dp->d_name[1] == '.' && dp->d_name[2] == '\0'))) continue;
      fpath = mystrcat(dirpath, "/", dp->d_name , NULL);
      if (lstat(fpath,&stbuf) == -1) errorexit("lstat(%s)",fpath);

      // Catalogue line
      if (dat->catfp && dat->catfmt)
	fprintf(dat->catfp,dat->catfmt,
		stbuf.st_ino, filetype(stbuf.st_mode), stbuf.st_mode & ~S_IFMT,
		stbuf.st_nlink, stbuf.st_uid, stbuf.st_gid,
		stbuf.st_size, stbuf.st_blocks, stbuf.st_mtime,
		cdir, cdir[0] ? "/" : "", dp->d_name);

      //ckpt(0);
      if (!S_ISLNK(stbuf.st_mode)) {
	if (S_ISDIR(stbuf.st_mode) && stbuf.st_dev == rootdev) {
	  s = mystrcat( cdir,  cdir[0] ? "/" : "", dp->d_name, NULL);
	  utarray_push_back(dirs, &s);
	  free(s);
	} else if (S_ISREG(stbuf.st_mode)) {
	  if (stbuf.st_size == 0) continue;
	  if (inodetab_add(dat->itab, stbuf.st_ino,
			   mystrcat(cdir,cdir[0] ? "/" : "", dp->d_name, NULL),
			   stbuf.st_nlink, stbuf.st_mtime) == 1) {
	    // This is a new node
	    //ckpt(0);
	    duptab_add(dat->dtab, &stbuf, 0, NULL, NULL);
	    //ckpt("%lx\n",(long)dat->cache);
	    // validate cache...
	    if (dat->cache) hcache_validate(dat->cache, &stbuf);
	    //ckpt(0);
	  }
	}
      }
      free(fpath);
    }
    //ckpt(0);
    closedir(dh);
    free(dirpath);
    free(cdir);
    //ckpt(0);
  }
  //ckpt(0);
  utarray_free(dirs);
}
