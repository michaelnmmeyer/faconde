#!/usr/bin/env sh

set -o errexit

VG="valgrind --leak-check=full --error-exitcode=1"

for file in *.lua; do
   $VG lua -e 'package.cpath="../lua/?.so"' ./$file
done

exit 0
