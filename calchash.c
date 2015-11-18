#include "calchash.h"
#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#ifndef HASH_TYPE
#define HASH_TYPE MD5
#endif

//////////////////////////////////////////////////////////////////////
//
// Implements MD5
//
//////////////////////////////////////////////////////////////////////
#include "lib/crypto-algorithms-master/md5.h"
#include "lib/crypto-algorithms-master/md5.c"
#define MD5_HLEN 16

struct md5_hash_ctx {
  MD5_CTX c;
};
static char md5_name[] = "md5";

static struct hash_ctx *md5_hash_new() {
  struct md5_hash_ctx *ctx = mymalloc(sizeof(struct md5_hash_ctx));
  memset(ctx,0,sizeof(struct md5_hash_ctx));
  md5_init(&(ctx->c));
  return (struct hash_ctx *)ctx;
}

static void md5_hash_update(struct hash_ctx *ctxp,char *data,int len) {
  struct md5_hash_ctx *ctx = (struct md5_hash_ctx *)ctxp;
  md5_update(&ctx->c,(BYTE *)data,len);
}
static void md5_hash_free(struct hash_ctx *ctxp, char *hash) {
  struct md5_hash_ctx *ctx = (struct md5_hash_ctx *)ctxp;
  if (hash) md5_final(&ctx->c,(BYTE *)hash);
  free(ctx);
}


//////////////////////////////////////////////////////////////////////
//
// Implements SHA256/SHA2
//
//////////////////////////////////////////////////////////////////////
//
// Implements SHA-256 (SHA-2)
//
#undef ROTLEFT
#include "lib/crypto-algorithms-master/sha256.h"
#include "lib/crypto-algorithms-master/sha256.c"
#define SHA256_HLEN 32

struct sha256_hash_ctx {
  SHA256_CTX c;
};

static char sha256_name[] = "SHA256";

static struct hash_ctx *sha256_hash_new() {
  struct sha256_hash_ctx *ctx = mymalloc(sizeof(struct sha256_hash_ctx));
  memset(ctx,0,sizeof(struct sha256_hash_ctx));
  sha256_init(&ctx->c);
  return (struct hash_ctx *)ctx;
}
static void sha256_hash_update(struct hash_ctx *ctxp,char *data,int len) {
  struct sha256_hash_ctx *ctx = (struct sha256_hash_ctx *)ctxp;
  sha256_update(&ctx->c,(BYTE *)data,len);
}
static void sha256_hash_free(struct hash_ctx *ctxp, char *hash) {
  struct sha256_hash_ctx *ctx = (struct sha256_hash_ctx *)ctxp;
  if (hash) sha256_final(&ctx->c,(BYTE *)hash);
  free(ctx);
}
//////////////////////////////////////////////////////////////////////

struct IHash calchash = {
  .hash_type = MD5,
  .hash_len = MD5_HLEN,
  .hash_new_fn = md5_hash_new,
  .hash_update_fn = md5_hash_update,
  .hash_free_fn = md5_hash_free,
  .hash_name = md5_name,
};

void hash_set(int type) {
  switch (type) {
  case MD5:
    calchash.hash_type = MD5;
    calchash.hash_len = MD5_HLEN;
    calchash.hash_new_fn = md5_hash_new;
    calchash.hash_update_fn = md5_hash_update;
    calchash.hash_free_fn = md5_hash_free;
    calchash.hash_name = md5_name;
    break;
  case SHA256:
    calchash.hash_type = SHA256;
    calchash.hash_len = SHA256_HLEN;
    calchash.hash_new_fn = sha256_hash_new;
    calchash.hash_update_fn = sha256_hash_update;
    calchash.hash_free_fn = sha256_hash_free;
    calchash.hash_name = sha256_name;
    break;
  default:
    fatal(EINVAL,"Invalid hash type %x\n", type);
  }
}

//////////////////////////////////////////////////////////////////////
char *hash_file(const char *file) {
  int fd, cnt;
  struct hash_ctx *ctx;
  char buf[_HASH_BUFSZ], *res;

  fd = open(file, O_RDONLY);
  if (fd == -1) errorexit("open(%s)", file);

  ctx = hash_new();
  while ((cnt = read(fd,buf,sizeof buf)) == 0) {
    if (cnt == -1) errorexit("read(%s)",file);
    hash_update(ctx, buf, cnt);
  }
  res = mymalloc(hash_len());
  hash_free(ctx,res);
  return res;
}
