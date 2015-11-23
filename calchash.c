#include "calchash.h"
#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BYTE MD2_BYTE
#define WORD MD2_WORD
#include <md2.h>
#include <md2.c>
#undef BYTE
#undef WORD
#define BYTE MD5_BYTE
#define WORD MD5_WORD
#include <md5.h>
#include <md5.c>
#undef BYTE
#undef WORD
#undef ROTLEFT
#define BYTE SHA1_BYTE
#define WORD SHA1_WORD
#include <sha1.h>
#include <sha1.c>
#undef BYTE
#undef WORD
#undef ROTLEFT
#define BYTE SHA256_BYTE
#define WORD SHA256_WORD
#include <sha256.h>
#include <sha256.c>

#define declare_hash(type,id,libctx,bs)					\
  struct ctx_ ## id {							\
    libctx c;								\
  };									\
  static char id ## _name[] = #id;					\
  static struct hash_ctx * id ## _hash_new() {				\
    struct ctx_ ## id *ctx = mymalloc(sizeof(struct ctx_ ## id));	\
    memset(ctx,0,sizeof(struct ctx_ ## id));				\
    id ## _init(&(ctx->c));						\
    return (struct hash_ctx *)ctx;					\
  }									\
  static void id ## _hash_update(struct hash_ctx *cp, char *data,int len) { \
    struct ctx_ ## id *ctx = (struct ctx_ ## id *)cp;			\
    id ## _update(&ctx->c,(BYTE *)data,len);				\
  }									\
  static void id ## _hash_free(struct hash_ctx *cp, char *hash) {	\
    struct ctx_ ## id *ctx = (struct ctx_ ## id *)cp;			\
    if(hash) id ## _final(&ctx->c,(BYTE *)hash);			\
    free(ctx);								\
  }									\
  static void id ## _hash_init(struct IHash *p) {			\
    p->hash_type = type;						\
    p->hash_len = bs;							\
    p->hash_new_fn = id ## _hash_new;					\
    p->hash_update_fn = id ## _hash_update;				\
    p->hash_free_fn = id ## _hash_free;					\
    p->hash_name = id ## _name;						\
  }

//////////////////////////////////////////////////////////////////////
declare_hash(CH_MD2, md2, MD2_CTX, MD2_BLOCK_SIZE)
declare_hash(CH_MD5, md5, MD5_CTX, MD5_BLOCK_SIZE)
declare_hash(CH_SHA256, sha256, SHA256_CTX, SHA256_BLOCK_SIZE)
declare_hash(CH_SHA1, sha1, SHA1_CTX, SHA1_BLOCK_SIZE)
//////////////////////////////////////////////////////////////////////
struct IHash calchash;
void hash_set(int type) {
  switch (type) {
  case CH_MD2:
    md2_hash_init(&calchash);
    break;
  case CH_MD5:
    md5_hash_init(&calchash);
    break;
  case CH_SHA1:
    sha1_hash_init(&calchash);
    break;
  case CH_SHA256:
    sha256_hash_init(&calchash);
    break;
  default:
    fatal(EINVAL,"Invalid hash type %x\n", type);
  }
}

//////////////////////////////////////////////////////////////////////
#define _HASH_BUFSZ 4096

char *hash_file(const char *file) {
  int fd, cnt;
  struct hash_ctx *ctx;
  char buf[_HASH_BUFSZ], *res;

  fd = open(file, O_RDONLY);
  if (fd == -1) errormsg("open(%s)", file);

  ctx = hash_new();
  while ((cnt = read(fd,buf,sizeof buf)) > 0) {
    if (cnt == -1) errormsg("read(%s)",file);
    hash_update(ctx, buf, cnt);
  }
  close(fd);
  res = mymalloc(hash_len());
  hash_free(ctx,res);
  return res;
}
