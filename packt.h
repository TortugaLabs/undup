#ifndef _PACKT_H
#define _PACKT_H
#include "utils.h"

char *unpackt(char *buffer,const char *tpl,...);
char *_packt(int twice,int *dsize,char *buffer,const char *tpl,...);

#define packt(szptr,tpl,...) _packt(true,szptr,NULL,tpl, ## __VA_ARGS__)
#define packtsz(szptr,tpl,...) _packt(false,szptr,NULL,tpl, ## __VA_ARGS__)
#define packtcp(szptr,buf,tpl,...) _packt(false,szptr,buf,tpl,## __VA_ARGS__)
#endif
