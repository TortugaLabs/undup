#ifndef _DUPTABLE_H
#define _DUPTABLE_H
#include <sys/types.h>
#include <sys/stat.h>

struct duptab;
struct duptab *duptab_new();
struct duptab *duptab_free(struct duptab *tab);
void duptab_add(struct duptab *tab,struct stat *stdat,int hlen,void *hash);
void duptab_sort(struct duptab *tab);
int duptab_count(struct duptab *tab);

ino_t *duptab_first(struct duptab *tab,int *cnt,struct stat *st);
ino_t *duptab_next(struct duptab *tab,int *cnt, struct stat *st);

#ifdef _DEBUG
void duptab_dump(struct duptab *tab);
#else
#define duptab_dump(tab) ((void)0)
#endif
#endif
