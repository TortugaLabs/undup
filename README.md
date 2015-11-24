# undup

Track duplicate files and merge them as hardlinks

Undup is like a number of other command-line utilities that is
intended to find duplicates in a given set of files and replace them
from hardlinks to save space.

There are plenty of these types of utilities.  I originally wrote this
in PHP because I need to run it on a NAS that provided a PHP
interpreter.  In order to achieve a decent performance this cached
file hashes.  Because the size of the data increased, I wanted to
improve the performance of this code by re-writting it in C.

Its only unique feature (that distinguishes it from the other
alternatives referred here) is the use of file to cache hashes.

## Alternatives

Few alternatives:

* [duff](http://duff.dreda.org/) by Camilla Berglund.
* [rdfind](http://rdfind.pauldreik.se/) by Paul Dreik
* [fdupes](https://github.com/adrianlopezroche/fdupes) by Adrian Lopez

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


## Changes

* 2.0.0: C rewrite
  - Re-wrote it in C
* 1.0.0: Initial PHP release

## Copyright

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
