/*
 *    This file is part of undup
 *    Copyright (C) 2015, Alejandro Liu
 *
 *    undup is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    undup is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, see <http://www.gnu.org/licenses>
 */
#include <cu.h>
#include "calchash.h"
#include "utils.h"

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

TEST(sha1check_and_file) {
  hash_set(CH_SHA1);
  char p[] = "shaXXXXXX";
  if (mkstemp(p) == -1) {
    assertFalse("mkstemp failed");
    return;
  }
  char text[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
  BYTE hash[] = {0x84,0x98,0x3e,0x44,0x1c,0x3b,0xd2,0x6e,0xba,0xae,0x4a,0xa1,0xf9,0x51,0x29,0xe5,0xe5,0x46,0x70,0xf1};

  FILE *fp = fopen(p,"w");
  assertTrue(fp);
  if (fp) {
    fputs(text,fp);
    fclose(fp);
  }
  char *buf = hash_file(p);
  assertTrue(!memcmp(hash,buf,hash_len()));
  //printhex(stdout,buf,hash_len(),0);
  unlink(p);
  free(buf);
  printf("%s\n", hash_name());
}

TEST(sha256check) {
  hash_set(CH_SHA256);
  char text1[] = {"abc"};
  char text2[] = {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"};
  char text3[] = {"aaaaaaaaaa"};
  BYTE hash1[] = {0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
		  0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad};
  BYTE hash2[] = {0x24,0x8d,0x6a,0x61,0xd2,0x06,0x38,0xb8,0xe5,0xc0,0x26,0x93,0x0c,0x3e,0x60,0x39,
		  0xa3,0x3c,0xe4,0x59,0x64,0xff,0x21,0x67,0xf6,0xec,0xed,0xd4,0x19,0xdb,0x06,0xc1};
  BYTE hash3[] = {0xcd,0xc7,0x6e,0x5c,0x99,0x14,0xfb,0x92,0x81,0xa1,0xc7,0xe2,0x84,0xd7,0x3e,0x67,
		  0xf1,0x80,0x9a,0x48,0xa4,0x97,0x20,0x0e,0x04,0x6d,0x39,0xcc,0xc7,0x11,0x2c,0xd0};

  char *buf;
  struct hash_ctx *ctx;
  buf = mymalloc(hash_len());

  ctx = hash_new();
  hash_update(ctx, text1,strlen(text1));
  hash_free(ctx,buf);
  assertTrue(!memcmp(hash1,buf,hash_len()));

  ctx = hash_new();
  hash_update(ctx, text2,strlen(text2));
  hash_free(ctx,buf);
  assertTrue(!memcmp(hash2,buf,hash_len()));

  // Note the data is being added in mutiple chunks chunks.
  ctx = hash_new();
  for (int i=0; i< 100000; ++i)
    hash_update(ctx, text3,strlen(text3));
  hash_free(ctx,buf);
  assertTrue(!memcmp(hash3,buf,hash_len()));
  free(buf);
  printf("%s\n", hash_name());
}

TEST(md5check) {
  hash_set(CH_MD5);

  char text1[] = "";
  char text2[] = "abc";
  char text3_1[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcde";
  char text3_2[] = "fghijklmnopqrstuvwxyz0123456789";
  BYTE hash1[] = {0xd4,0x1d,0x8c,0xd9,0x8f,0x00,0xb2,0x04,0xe9,0x80,0x09,0x98,0xec,0xf8,0x42,0x7e};
  BYTE hash2[] = {0x90,0x01,0x50,0x98,0x3c,0xd2,0x4f,0xb0,0xd6,0x96,0x3f,0x7d,0x28,0xe1,0x7f,0x72};
  BYTE hash3[] = {0xd1,0x74,0xab,0x98,0xd2,0x77,0xd9,0xf5,0xa5,0x61,0x1c,0x2c,0x9f,0x41,0x9d,0x9f};
  char *buf;

  struct hash_ctx *ctx;
  buf = mymalloc(hash_len());

  ctx = hash_new();
  hash_update(ctx, text1,strlen(text1));
  hash_free(ctx,buf);
  assertTrue(!memcmp(hash1,buf,hash_len()));

  ctx = hash_new();
  hash_update(ctx, text2,strlen(text2));
  hash_free(ctx,buf);
  assertTrue(!memcmp(hash2,buf,hash_len()));

  // Note the data is being added in two chunks.
  ctx = hash_new();
  hash_update(ctx, text3_1,strlen(text3_1));
  hash_update(ctx, text3_2,strlen(text3_2));
  hash_free(ctx,buf);
  assertTrue(!memcmp(hash3,buf,hash_len()));
  free(buf);

  printf("%s\n", hash_name());
}
