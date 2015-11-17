#ifndef _DUPTABLE_H
#define _DUPTABLE_H
#include <sys/types.h>
#include <sys/stat.h>

struct duptab;
struct duptab *duptab_new();
void duptab_free(struct duptab *tab);
void duptab_add(struct duptab *tab,struct stat *stdat,int hlen,void *shash,void *lhash);
void duptab_sort(struct duptab *tab);

ino_t *duptab_first(struct duptab *tab,int *cnt,struct stat *st);
ino_t *duptab_next(struct duptab *tab,int *cnt, struct stat *st);

#ifdef XDEBUG
void duptab_dump(struct duptab *tab);
#else
#define duptab_dump(tab) ((void)0)
#endif
#endif
