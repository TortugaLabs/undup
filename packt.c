#include "utils.h"
#include <stdarg.h>
#include <string.h>

#define return_type(intval)			\
  do { switch(*tpl++) {				\
 case 'i':					\
 intptr = va_arg(ap,int *);			\
 *intptr = intval;				\
 break;						\
 case 's':					\
 sptr = va_arg(ap,short *);			\
 *sptr = (short)intval;				\
 break;						\
 case 'c':					\
 cptr = va_arg(ap,char *);			\
 *cptr = (char)intval;				\
 break;						\
 default:					\
 fatal(EINVAL,"Invalid type");			\
    } } while (0)

char *unpackt(char *buffer,const char *tpl,...) {
  va_list ap;
  char *p = buffer;
  int intval, *intptr, ln;
  short *sptr;
  char *cptr;
  char **s;
  int cnt = 0;

  va_start(ap,tpl);
  while (*tpl) {
    switch (*tpl++) {
    case '1':
      intval = (unsigned char)(*(p++));
      return_type(intval);
      cnt++;
      break;
    case '2':
      intval = (((unsigned char)(*(p++)) << 8) & 0xff00);
      intval |= ((unsigned char)(*(p++)) & 0xff);
      return_type(intval);
      cnt++;
      break;
    case '4':
      intval = (((unsigned char)(*(p++)) << 24) & 0xff000000);
      intval |= (((unsigned char)(*(p++)) << 16) & 0xff0000);
      intval |= (((unsigned char)(*(p++)) << 8) & 0xff00);
      intval |= ((unsigned char)(*(p++)) & 0xff);
      return_type(intval);
      cnt++;
      break;
    case 'S':
      s = va_arg(ap,char **);
      ln = *(p++) & 0xff;
      if ((ln & 0x80) == 0x80)
	ln = ((ln & 0x7f) << 8) | ( *(p++) & 0xff);
      *s = (char *)mymalloc(ln);
      memcpy(*s,p,ln);
      p += ln;
      cnt++;
      break;
    case 'b':
      ln = va_arg(ap,int);
      s = va_arg(ap,char **);
      *s = (char *)mymalloc(ln);
      memcpy(*s,p,ln);
      p += ln;
      cnt++;
      break;
    case 'i': // Ignore these...
    case 's':
    case 'c':
      break;
    default:
      fatal(EINVAL,"Invalid template");
    }
  }
  return p;
}

// size only
// size and malloc
// copy only

char *_packt(int twice,int *dsize,char *buffer,const char *tpl,...) {
  va_list ap;
  int intval, ln;
  char *s;
  char *p;
  int repeat;
  long long int8val;

  do {
    repeat = false;
    *dsize = 0;
    p = buffer;
    va_start(ap,tpl);
    while (*tpl) {
      switch (*tpl++) {
      case '1':
	intval = va_arg(ap, int);
	if (buffer) {
	  *(p++) = (unsigned char)intval;
	}
	(*dsize)++;
	break;
      case '2':
	intval = va_arg(ap, int);
	if (buffer) {
	  *(p++) = (unsigned char)((intval>>8) & 0xff);
	  *(p++) = (unsigned char)(intval & 0xff);
	}
	(*dsize) += 2;
	break;
      case '4':
	intval = va_arg(ap, int);
	if (buffer) {
	  *(p++) = (unsigned char)((intval>>24) & 0xff);
	  *(p++) = (unsigned char)((intval>>16) & 0xff);
	  *(p++) = (unsigned char)((intval>>8) & 0xff);
	  *(p++) = (unsigned char)(intval & 0xff);
	}
	(*dsize) += 4;
	break;
      case '8':
	int8val = va_arg(ap, long long);
	if (buffer) {
	  *(p++) = (unsigned char)((int8val>>56) & 0xff);
	  *(p++) = (unsigned char)((int8val>>48) & 0xff);
	  *(p++) = (unsigned char)((int8val>>40) & 0xff);
	  *(p++) = (unsigned char)((int8val>>32) & 0xff);

	  *(p++) = (unsigned char)((int8val>>24) & 0xff);
	  *(p++) = (unsigned char)((int8val>>16) & 0xff);
	  *(p++) = (unsigned char)((int8val>>8) & 0xff);
	  *(p++) = (unsigned char)(int8val & 0xff);
	}
	(*dsize) += 8;
	break;
      case 'S':
	s = va_arg(ap,char *);
	ln = strlen(s);
	if (buffer) {
	  if (ln < 128) {
	    *(p++) = (unsigned char)(ln);
	    memcpy(p,s,ln);
	    p += ln;
	  } else {
	    *(p++) = (unsigned char) (((ln >> 8) & 0x7f) | 0x80);
	    *(p++) = (unsigned char) (ln & 0xff);
	    memcpy(p,s,ln);
	    p += ln;
	  }
	}
	(*dsize) += (ln < 128 ? 1 : 2) + ln;
	break;
      case 'i': // Ignore these...
      case 's':
      case 'c':
	break;
      case 'b':
	ln = va_arg(ap,int);
	s = va_arg(ap, char *);
	if (buffer) {
	  memcpy(p,s,ln);
	  p += ln;
	}
	(*dsize) += ln;
	break;
      default:
	fatal(EINVAL,"Invalid template");
      }
      if (twice) {
	twice = false;
	repeat = true;
	if (buffer == NULL) buffer = mymalloc(*dsize);
      }
    }
  } while (repeat);
  return buffer;
}

