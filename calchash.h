#ifndef _CALCHASH_H
#define _CALCHASH_H
#define _HASH_BUFSZ 4096
struct hash_ctx;

int hash_type();
int hash_len();
struct hash_ctx *hash_new();
void hash_init(struct hash_ctx *ctx);
void hash_update(struct hash_ctx *ctx,char *data,int len);
char *hash_get(struct hash_ctx *ctx);
void hash_free(struct hash_ctx *ctx, char *hash);

char *hash_part(const char *file,const char *root);
char *hash_full(const char *file,const char *root);
#endif
