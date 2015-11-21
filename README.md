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

    Copyright (c) 2015, Alejandro Liu
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of undup nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
    CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
    BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
    TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
    THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
