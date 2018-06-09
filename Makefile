#
#    This file is part of undup
#    Copyright (C) 2015, Alejandro Liu
#
#    undup is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    undup is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, see <http://www.gnu.org/licenses>
#
#_begin := $(shell ./scripts/init.sh)

TARGET = #arm-mv5sft-linux-gnueabi
CC = $(TARGET)gcc
LD = $(TARGET)gcc
AR = $(TARGET)ar
CFG_TARGET= # --host=$(TARGET)

EXTLIBDIR = lib
PEDANTIC=-Wall -Wextra -Werror -std=gnu99 -pedantic
INCDIRS = -I$(CRYPTO_DIR) -I$(UTHASH_DIR)/src $(GDBM_INCLUDE)

GOPTZ = -Og
OPTIMIZ = -g -D_DEBUG $(GOPTZ) # Debug build
#OPTIMIZ = -O3 # Prod build

CFLAGS = $(OPTIMIZ) $(PEDANTIC) $(INCDIRS) $(XCFLAGS)
#-O2 -I$(UTHASH_DIR)/src -I$(GDBM_LIBDIR)/src #-DHASH_TYPE=SHA256

GDBM_VERSION=1.12
GDBM_UNPACK=[ -n "$(GDBM_DEP)" ] && ([ -d $(GDBM_LIBDIR) ] || ( cd $(EXTLIBDIR) && tar zxf gdbm-$(GDBM_VERSION).tar.gz )) || :
GDBM_LIBDIR=$(EXTLIBDIR)/gdbm-$(GDBM_VERSION)

ifdef EMBED_GDBM
GDBM_INCLUDE=-Ilib/gdbm-$(GDBM_VERSION)/src
GDBM_DEP=$(GDBM_LIBDIR)/libgdbm.a
GDBM_LIBREF=$(GDBM_DEP)
else
GDBM_LIBREF=-lgdbm
endif

UTHASH_DIR = $(EXTLIBDIR)/uthash
CRYPTO_DIR = $(EXTLIBDIR)/crypto-algorithms
CU_DIR = $(EXTLIBDIR)/cu
CU_CFLAGS = -I$(CU_DIR)

OBJS = utils.o calchash.o inodetab.o duptable.o hcache.o \
	undup.o fscanner.o dedup.o human_readable.o
TESTS = $(shell for f in $(OBJS:%.o=%-t.c) ; do [ -f $$f ] && echo $$f ; done)
# MTRACE = env MALLOC_TRACE=mtrace.data

help:
	@echo "Use:"
	@echo "	make prod - production version"
	@echo "	make debug - debugging/development version"
	@echo "	make check - run tests"

all: undup docs

prod:
	# This macro checks that we have do not have debug build stuff
	# and/or using the correct target architecture
	if [ -f _debug ] ; then  make realclean ; fi
	set -x ; t=$(TARGET) ; [ -z "$$t" ] && t=`uname -m` ; \
		if [ -f _prod ] ; then \
		  c=$$(cat _prod) ; \
		  [ x"$$c" != x"$$t" ] && make realclean ; \
		fi ; \
		echo $$t > _prod
	make OPTIMIZ=-O3 LDFLAGS=-s all

debug:
	# This macro checks that we have do not have prod build stuff
	# and/or using the correct target architecture
	if [ -f _prod ] ; then  make realclean ; fi
	t=$(TARGET) ; [ -z "$$t" ] && t=`uname -m` ; \
		if [ -f _debug ] ; then \
		  c=$$(cat _debug) ; \
		  [ x"$$c" != x"$$t" ] && make realclean ; \
		fi ; \
		echo $$t > _debug
	make all

docs: undup.1 undup.adoc

undup.1: undup.c
	type manify && manify undup.c || true

undup.adoc: undup.c
	type manify && (manify --asciidoc undup.c > undup.adoc || rm -f undup.adoc) || true

cu-check-regressions: $(EXTLIBDIR)/cu-patch $(CU_DIR)/cu-check-regressions
	patch -o cu-check-regressions $(CU_DIR)/cu-check-regressions $(EXTLIBDIR)/cu-patch || ( rm -f cu-check-regressions ; exit 1)
	chmod 755 cu-check-regressions

check:
	# This macro checks that we have do not have prod build stuff
	# and/or using the correct target architecture
	if [ -f _prod ] ; then  make realclean ; fi
	t=$(TARGET) ; [ -z "$$t" ] && t=`uname -m` ; \
		if [ -f _debug ] ; then \
		  c=$$(cat _debug) ; \
		  [ x"$c" != x"$t" ] && make realclean ; \
		fi ; \
		echo $$t > _debug
	make _check

_check: test cu-check-regressions
	[ -d regressions ] || mkdir regressions
	$(MTRACE) ./test
	./cu-check-regressions regressions

test: test.c $(OBJS) $(TESTS) $(GDBM_DEP)
	./scripts/gentests -o cu-t.h $(TESTS)
	$(CC) $(CFLAGS) $(CU_CFLAGS) -o test \
		test.c $(OBJS) $(TESTS) $(GDBM_LIBREF)

undup: vcheck $(OBJS) main.o $(GDBM_DEP)
	$(LD) $(LDFLAGS) $(CFLAGS) -o undup main.o $(OBJS) $(GDBM_LIBREF)

vcheck:
	: VCHECK
	if type git ; then \
	  vname="$$(git describe --dirty=_Exp)" ; \
	  if [ -n "$$vname" ] ; then \
	    echo "const char version[] = \"$$vname\";" > version.h.t ;\
	    if cmp version.h.t version.h ; then \
	      rm -f version.h.t ; \
	    else \
	      mv version.h.t version.h ; \
	    fi ;\
	  fi ;\
	fi

# pull in dependancy...
-include $(OBJS:.o=.d)

%.o: %.c
	# Make sure other sources are there...
	$(GDBM_UNPACK)
	# compile and generate dependancy info
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(CC) -MM $(CFLAGS) $*.c > $*.d

lib/gdbm-$(GDBM_VERSION)/libgdbm.a:
	$(GDBM_UNPACK)
	cd $(GDBM_LIBDIR) && ./configure $(CFG_TARGET) && make 
	cd $(GDBM_LIBDIR) && $(AR) cr libgdbm.a src/*.o

clean:
	rm -f *.o *.d mtrace.data
	rm -f test cu-t.h regressions/tmp.* cu-check-regressions
	rm -f undup

realclean: clean
	rm -rf $(GDBM_LIBDIR) _debug _prod
