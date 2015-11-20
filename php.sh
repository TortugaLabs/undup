#!/bin/sh
export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/zy-pkgs/lib
exec /usr/local/zy-pkgs/php/bin/php "$@"
