#ifndef _HCACHE_H
#define _HCACHE_H
#include <sys/types.h>
#include <sys/stat.h>

struct hcache;
struct hcache *hcache_new(const char *base,int type,int len);
struct hcache *hcache_free(struct hcache *cache);
void hcache_validate(struct hcache *cache, struct stat *st);
int hcache_get(struct hcache *cache, struct stat *st,char **hash);
void hcache_put(struct hcache *cache, struct stat *st,char *hash);
void hcache_del(struct hcache *cache, struct stat *st);
const char *hcache_getpath(struct hcache *cache);

void hcache_stats(struct hcache *cache,int *hits, int *misses);
#endif
