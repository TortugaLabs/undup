#ifndef _INODETAB_H
#define _INODETAB_H
#include <sys/types.h>

struct inodetab;
struct inodetab *inodetab_new();
struct inodetab *inodetab_free(struct inodetab *tab);
int inodetab_add(struct inodetab *tab,ino_t ino,char *fpath,int nlnks,time_t mtime);
char **inodetab_get(struct inodetab *tab,ino_t ino,time_t *mtime);
int inodetab_count(struct inodetab *tab);

#ifdef _DEBUG
void inodetab_dump(struct inodetab *tab);
#else
#define inodetab_dump(tab) ((void)0)
#endif
#endif
