#!/bin/sh
#
# Basic dedup script
#
export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/zy-pkgs/lib
script_dir=$(cd $(dirname $0) ; pwd)
exec /usr/local/zy-pkgs/php/bin/php $script_dir/undup.php "$@"
