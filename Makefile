#_begin := $(shell ./scripts/init.sh)

OBJS := main.o fscanner.o inodetab.o duptable.o packt.o calchash.o dedup.o hcache.o
TARGET = #arm-mv5sft-linux-gnueabi
CC = $(TARGET)gcc
LD = $(TARGET)gcc
AR = $(TARGET)ar
HOST_TARGET= # --host=$(TARGET)
CFLAGS = -Wall -O2 -I$(UTHASH_DIR)/src -I$(GDBM_LIBDIR)/src #-DHASH_TYPE=SHA256
GDBM_VERSION=1.11
GDBM_LIBDIR=lib/gdbm-$(GDBM_VERSION)
GDBM_UNPACK=[ -d $(GDBM_LIBDIR) ] || ( cd lib && tar zxf gdbm-$(GDBM_VERSION).tar.gz )
LIBDEPS = $(GDBM_LIBDIR)/libgdbm.a
UTHASH_DIR=lib/uthash-master
CRYPTO_DIR=lib/crypto-algorithms-master

# link...
undup: $(LIBDEPS) $(OBJS)
	$(LD) $(OBJS) -L$(GDBM_LIBDIR) -lgdbm -o undup

# pull in dependancy...
-include $(OBJS:.o=.d)

# compile and generate dependancy info
%.o: %.c
	$(GDBM_UNPACK)
	[ -d $(UTHASH_DIR) ] || ( cd lib && unzip -q uthash-master.zip )
	[ -d $(CRYPTO_DIR) ] || ( cd lib && unzip -q crypto-algorithms-master.zip )
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(CC) -MM $(CFLAGS) $*.c > $*.d

$(GDBM_LIBDIR)/libgdbm.a:
	$(GDBM_UNPACK)
	cd $(GDBM_LIBDIR) && ./configure $(HOST_TARGET) && make
	cd $(GDBM_LIBDIR) && $(AR) cr libgdbm.a src/*.o

clean:
	rm -f undup *.o *.d

realclean: clean
	rm -rf $(GDBM_LIBDIR) $(UTHASH_DIR) $(CRYPTO_DIR)
