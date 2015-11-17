#include "calchash.h"
#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MD5 '5'
#define SHA256 '2'
#ifndef HASH_TYPE
#define HASH_TYPE MD5
#endif

#if HASH_TYPE == MD5
//
// Implements MD5
//
#include "lib/crypto-algorithms-master/md5.h"
#include "lib/crypto-algorithms-master/md5.c"
#define HLEN 16

struct hash_ctx {
  MD5_CTX c;
  char h[HLEN];
};

int hash_type() {
  return MD5;
}
int hash_len() {
  return HLEN;
}

void hash_init(struct hash_ctx *ctx) {
  memset(ctx,0,sizeof(struct hash_ctx));
  md5_init(&ctx->c);
}
struct hash_ctx *hash_new() {
  struct hash_ctx *ctx = mymalloc(sizeof(struct hash_ctx));
  hash_init(ctx);
  return ctx;
}
void hash_update(struct hash_ctx *ctx,char *data,int len) {
  md5_update(&ctx->c,(BYTE *)data,len);
}
char *hash_get(struct hash_ctx *ctx) {
  md5_final(&ctx->c,(BYTE *)ctx->h);
  return ctx->h;
}
void hash_free(struct hash_ctx *ctx, char *hash) {
  if (hash) md5_final(&ctx->c,(BYTE *)hash);
  free(ctx);
}


#endif


#if HASH_TYPE == SHA256
//
// Implements SHA-256 (SHA-2)
//
#include <string.h>
#include "lib/crypto-algorithms-master/sha256.h"
#include "lib/crypto-algorithms-master/sha256.c"
#define HLEN 32

struct hash_ctx {
  SHA256_CTX c;
  char h[HLEN];
};

int hash_type() {
  return SHA256;
}
int hash_len() {
  return HLEN;
}

void hash_init(struct hash_ctx *ctx) {
  memset(ctx,0,sizeof(struct hash_ctx));
  sha256_init(&ctx->c);
}
struct hash_ctx *hash_new() {
  struct hash_ctx *ctx = mymalloc(sizeof(struct hash_ctx));
  hash_init(ctx);
  return ctx;
}
void hash_update(struct hash_ctx *ctx,char *data,int len) {
  sha256_update(&ctx->c,(BYTE *)data,len);
}
char *hash_get(struct hash_ctx *ctx) {
  sha256_final(&ctx->c,(BYTE *)ctx->h);
  return ctx->h;
}
void hash_free(struct hash_ctx *ctx, char *hash) {
  if (hash) sha256_final(&ctx->c,(BYTE *)hash);
  free(ctx);
}
#endif

int _hash_open(const char *file,const char *root) {
  int fd;
  if (root) {
    char *vfile = mystrcat(root,"/",file,NULL);
    fd = open(vfile, O_RDONLY);
    if (fd == -1) errorexit("open(%s)", file);
    free(vfile);
  } else {
    fd = open(file, O_RDONLY);
    if (fd == -1) errorexit("open(%s)", file);
  }
  return fd;
}
char *hash_part(const char *file,const char *root) {
  int fd, cnt;
  struct hash_ctx *ctx;
  char buf[_HASH_BUFSZ], *res;

  fd = _hash_open(file, root);
  ctx = hash_new();
  cnt = read(fd,buf,sizeof buf);
  switch (cnt) {
  case -1:
    errorexit("read(%s)",file);
  case 0:
    fatal(ENODATA,"empty read(%s)",file);
  }
  hash_update(ctx, buf, cnt);
  res = mymalloc(hash_len());
  hash_free(ctx,res);
  return res;
}
char *hash_full(const char *file,const char *root) {
  int fd, cnt;
  struct hash_ctx *ctx;
  char buf[_HASH_BUFSZ], *res;

  fd = _hash_open(file,root);
  ctx = hash_new();
  while ((cnt = read(fd,buf,sizeof buf)) == 0) {
    if (cnt == -1) errorexit("read(%s)",file);
    hash_update(ctx, buf, cnt);
  }
  res = mymalloc(hash_len());
  hash_free(ctx,res);
  return res;
}
