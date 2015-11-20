# undup

Track duplicate files and merge them as hardlinks

Undup is like a number of other command-line utilities that is
intended to find duplicates in a given set of files and replace them
from hardlinks to save space.

There are plenty of these types of utilities.  I wrote this because I
wanted to run this on a NAS that provided a PHP interpreter.

It caches hashes in a ttempt to improve performance.

## Alternatives

Few alternatives:

* [duff](http://duff.dreda.org/) by Camilla Berglund.
* [rdfind](http://rdfind.pauldreik.se/) by Paul Dreik
* [fdupes](https://github.com/adrianlopezroche/fdupes) by Adrian Lopez
