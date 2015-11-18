#ifndef _CALCHASH_H
#define _CALCHASH_H
#define _HASH_BUFSZ 4096
struct hash_ctx;

#define MD5 '5'
#define SHA256 '2'

struct IHash {
  int hash_type;
  int hash_len;
  struct hash_ctx *(*hash_new_fn)();
  void (*hash_update_fn)(struct hash_ctx *ctx,char *data,int len);
  void (*hash_free_fn)(struct hash_ctx *ctx,char *hash);
  const char *hash_name;
};
extern struct IHash calchash;

void hash_set(int type);

#define hash_type() (calchash.hash_type)
#define hash_len() (calchash.hash_len)
#define hash_name() (calchash.hash_name)
#define hash_new() calchash.hash_new_fn()
#define hash_update(ctx,dat,len) calchash.hash_update_fn(ctx,dat,len)
#define hash_free(ctx,h) calchash.hash_free_fn(ctx,h)
char *hash_file(const char *file);
#endif
