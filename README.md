# undup

[![Build Status](https://api.travis-ci.org/alejandroliu/undup.png?branch=master)](http://travis-ci.org/alejandroliu/undup)

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

Currently, **undup** is only distributed in source form.  To compile
**undup** you need a **gcc-4.8** compiler.  I have only tested
building it with [Centos-7](https://www.centos.org/download/) and
[Ubuntu](http://www.ubuntu.com/).  To download the source from GitHub
and enter:

```
make prod
```

Then you can copy the resulting `undup` binary files to the
appropriate location (usually `/usr/bin`).  You can also copy the
provided `undup.1` man page.

I also the include the `XBUILD` script to create an executable for the
_ARM_ architecture which I use in my NAS device.  Customize this file
to taste.

## Documentation

The man page can be found [here](undup.adoc).

## Examples

To get a help page:

```
[alex@pc3 undup]$ ./undup -h
Usage: ./undup [options] dir
	-c catalogue: create a file catalogue
	-l lockfile: create a exclusive lock
	-C: disable hash caching
	-e: execute (disables dry-run mode)
	-m: shows memory stats
	-K: shows cache stats
	-s: scan only
	-5: use MD5 hashes
	-S: use SHA256 hashes
	-q: supress additional info
	-v: show additional info
	-V: version info
	-h|?: this help message

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
[alex@pc3 undup]$ ./undup -K -S ./data/test1
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
* [GDBM-1.11](http://www.gnu.org.ua/software/gdbm/) - library of
  database functions to create and manipulate a hashed database.
  Licensed as GPLv3.
* human_readable.c from [busybox](http://www.busybox.net/) - GPLv2
  Licensed.

## Issues

- duptable : implement a gdbm version and as you start loading check
  memory use and switch implementations.
- inodetab : implement a gdbm version and as you start loading check
  memory use and switch implementations.
- hcache
  - when validate, validate all hash types
  - write a in-memory version.
  - convert between two implementations
  - interface
- Add more hashes from [sha](http://www.saddi.com/software/sha/).
- MD2 and SHA1 are implemented but can not be selected.

## Alternatives

Few alternatives:

* [duff](http://duff.dreda.org/) by Camilla Berglund.
* [rdfind](http://rdfind.pauldreik.se/) by Paul Dreik
* [fdupes](https://github.com/adrianlopezroche/fdupes) by Adrian Lopez

## Continous Integration

This project makes use of [TravisCI](https://travis-ci.org/).  You can
find the build status page
[here](https://travis-ci.org/alejandroliu/undup).

## Changes

* 2.0.0: C rewrite
  - Re-wrote it in C
* 1.0.0: Initial PHP release

## License

**undup** is licensed as GPLv2 software.


    undup
    Copyright (C) 2015, Alejandro Liu

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
