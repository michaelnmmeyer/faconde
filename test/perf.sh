#!/usr/bin/env sh

WORDS=/usr/share/dict/words

LC_ALL=C sort -u $WORDS | ./perf
