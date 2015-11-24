#include "utils.h"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fscanner.h"
#include "undup.h"
#include "calchash.h"
#include <utarray.h>

static dev_t getfiledev(const char *f) {
  struct stat stbuf;
  if (stat(f,&stbuf) == -1) errormsg("lstat(%s)",f);
  return stbuf.st_dev;
}

void fscanner_init(struct fs_dat *fs,char *root,int usecache) {
  fs->root = root;
  fs->itab = inodetab_new();
  fs->dtab = duptab_new();
  fs->cache = usecache ? hcache_new(root,hash_type(),hash_len()) : NULL;
}

void fscanner_close(struct fs_dat *dat) {
  if (dat->itab) dat->itab = inodetab_free(dat->itab);
  if (dat->dtab) dat->dtab = duptab_free(dat->dtab);
  if (dat->cache) dat->cache = hcache_free(dat->cache);
}

void fscanner(struct fs_dat *dat, struct cat_cb *cb) {
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

    dirpath = mystrcat( root , cdir[0] ? "/" : "", cdir);
    dh = opendir(dirpath);
    if (dh == NULL) errormsg("opendir(%s)",dirpath);

    while ((dp = readdir(dh)) != NULL) {
      //ckpt(0);
      if (dp->d_name[0] == '.'
	&& (dp->d_name[1] == '\0'
		|| (dp->d_name[1] == '.' && dp->d_name[2] == '\0'))) continue;
      fpath = mystrcat(dirpath, "/", dp->d_name);
      if (lstat(fpath,&stbuf) == -1) errormsg("lstat(%s)",fpath);
      free(fpath);

      // Catalogue line
      if (cb) cb->callback(cdir, dp->d_name, &stbuf, cb->ext);
      //ckpt(0);
      if (!S_ISLNK(stbuf.st_mode)) {
	if (S_ISDIR(stbuf.st_mode) && stbuf.st_dev == rootdev) {
	  s = mystrcat( cdir,  cdir[0] ? "/" : "", dp->d_name);
	  utarray_push_back(dirs, &s);
	  free(s);
	} else if (S_ISREG(stbuf.st_mode)) {
	  char *p;
	  if (stbuf.st_size == 0) continue;
	  int i = inodetab_add(dat->itab, stbuf.st_ino,
			       p = mystrcat(cdir,cdir[0] ?"/" : "", dp->d_name),
			       stbuf.st_nlink, stbuf.st_mtime);
	  free(p);
	  if (i == 1) {
	    // This is a new node
	    //ckpt(0);
	    duptab_add(dat->dtab, &stbuf, 0, NULL);
	    //ckpt("%lx\n",(long)dat->cache);
	    // validate cache...
	    if (dat->cache) hcache_validate(dat->cache, &stbuf);
	    //ckpt(0);
	  }
	}
      }
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
