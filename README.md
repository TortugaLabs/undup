# undup

[![Build Status](https://api.travis-ci.org/TortugaLabs/undup.png?branch=master)](http://travis-ci.org/TortugaLabs/undup)

Track duplicate files and merge them as hardlinks

**undup** is like a number of other command-line utilities that is
intended to find duplicates in a given set of files and replace them
with hardlinks to save space.

There are plenty of these types of utilities.  I originally wrote this
in PHP because I need to run it on a NAS that provided a PHP
interpreter.  In order to achieve a decent performance this cached
file hashes.  Because the size, of the data increased, I wanted to
improve the performance of this code by re-writting it in C.

Its only unique feature (that distinguishes it from the other
alternatives referred here) is the use of file to cache hashes.

## Install

**undup** is distributed in source form and a tarball containing
a statically linked executable.
The _recommended_
way to install **undup** is to compile from source code.

To compile **undup** you need a **gcc-4.8** compiler.  I have only tested
building it with :

- [Centos-7](https://www.centos.org/)
- [AlpineLinux](http://www.alpinelinux.org)
- [Ubuntu](http://www.ubuntu.com/)
- [Void Linux](http://voidlinux.org/)

Download the source from GitHub and enter:

```
make prod
```

For static build:

```
make EMBED_GDBM=1 LXFLAGS=-static prod
```

Then you can copy the resulting `undup` binary files to the
appropriate location (usually `/usr/bin`).  You can also copy the
provided `undup.1` man page to the appropriate location.

I also the include the `XBUILD` script to create an executable for the
_ARM_ architecture which I use in my NAS device.  Customize this file
to taste.

### Build Options

- EMBED_GDBM=1 : Link with an embedded gdbm library
- LXFLAGS=-static : Build static binary

## Documentation

The man page can be found [here](undup.adoc).

## Examples

To get a help page:

```
[alex@pc3 undup]$ ./undup -h
Usage: ./undup [options] dir
	-5: use MD5 hashes (default)
	-C: disable hash caching
	-c catalogue: create a file catalogue
	-e: execute (disables dry-run mode)
	-H algo : Select hash algo
	-h|?: this help message
	-I pattern: include pattern
	-K: shows cache stats
	-l lockfile: create a exclusive lock
	-m: shows memory stats
	-q: supress additional info
	-S: use SHA256 hashes
	-s: scan only
	-V: version info
	-v: show additional info
	-X pattern: exclude pattern
```

Scan file system:

```
[alex@pc3 undup]$ ./undup -s ./data/test1
Using hash: md5
[SCAN-ONLY] Scanning ./data/test1
Files found: 21675
Size clusters found: 5025
```

Scan file system, showing memory statistics:

```
[alex@pc3 undup]$ ./undup -s -m ./data/test1
Using hash: md5
[SCAN-ONLY] Scanning ./data/test1
Files found: 21675
Size clusters found: 5025
```

De-duplicate a file system, using SHA256 checksums and showing cache stats:

```
[alex@pc3 undup]$ ./undup -e -K -S ./data/test1
Using hash: sha256
Scanning ./data/test1
Files found: 21675
Size clusters found: 5025
Files de-duped: 14553
Storage freed: 1.2G
Hash cache: 0:2429 (0% hit ratio)

```

More verbose output:

```
[alex@pc3 undup]$ ./undup -v -K -S ./data/test1/lb1
Using hash: sha256
Scanning ./data/test1/lb1
Files found: 9
Size clusters found: 3
Files de-duped: 3
Storage freed: 376.0K
- (180117f0):u=2001 g=1000 m=664 (323K):uthash-master.zip
    -> (2009570b):raro
    -> (2009570b):c/uthash-master.zip
- (180117f2):u=2001 g=1000 m=664 (52K):crypto-algorithms-master.zip
    -> (2009570a):c/crypto-algorithms-master.zip

```

## Additional Software

This software makes use of the following libraries:

* [uthash](https://github.com/troydhanson/uthash/) - in memory hash
  table. MIT style license.
* [cu](https://github.com/danfis/cu/) - C unit test library.  BSD
  Licensed.
* [crypto-algorithms](https://github.com/B-Con/crypto-algorithms/) -
  Basic implementations of standard cryptography algorithms, like AES, MD5,
  and SHA-1.  Public domain.
* [GDBM](http://www.gnu.org.ua/software/gdbm/) - library of
  database functions to create and manipulate a hashed database.
  Licensed as GPLv3.  The system gdbm library will be used unless
  EMBED_GDBM=1 is used during build.  This is the default for `XBUILD`.
* human_readable.c from [busybox](http://www.busybox.net/) - GPLv2
  Licensed.

## Alternatives

Few alternatives:

* [duff](http://duff.dreda.org/) by Camilla Berglund.
* [rdfind](http://rdfind.pauldreik.se/) by Paul Dreik
* [fdupes](https://github.com/adrianlopezroche/fdupes) by Adrian Lopez

## Continous Integration

This project makes use of [TravisCI](https://travis-ci.org/).  You can
find the build status page
[here](https://travis-ci.org/TortugaLabs/undup).

Releases are done using [TravisCI](https://travis-ci.org/).  Steps:

1. Update Change log in `README.md`
2. Update `version.h` to the new version.
3. Commit and push changes to [github](https://github.com)
4. Check in [TravisCI](https://travis-ci.org/) that build is succesful.
5. Create a new tag:
   - `git tag -a x.y.z -m x.y.z`
   - `git push --tags`

## Changes

* 2.2.3: WIP
  - updated documentation
* 2.2.2: Minor updates
  - compile with musl
  - add option to build static binary
  - inode in catalogue is unsigned
  - removing alpine releases in favor for a single static release
* 2.2.1: Maintenance release
  - Code clean-ups
  - Updating `alpine linux` release to v3.8.
* 2.2.0:
  - Added excludes/includes options
  - Added option -H to select all hashes
* 2.1.1:
  - Fixing tests
  - Updating libraries
* 2.1.0:
  - Updated Makefile so it is also possible to build using the current
    os libgdbm instead of the embedded copy using:
      - make GDBM_UNPACK=: GDBM_DEP= GDBM_REF=-lgdbm prod
  - Upgrading gdbm to v1.12  
* 2.0.2:
  - Automatically update version display
  - Some documentation clarification
* 2.0.1: Minor updates
  - Added error checks
  - updated documentation
* 2.0.0: C rewrite
  - Re-wrote it in C
* 1.0.0: Initial PHP release

## TODO

- Read and display /proc/self/status
- hcd file inside directory tree

## License

**undup** is licensed as GPLv2 software.


    undup
    Copyright (C) 2015, 2018 Alejandro Liu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, see <http://www.gnu.org/licenses>
