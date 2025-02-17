#!/usr/bin/env sh

set -x
clang -O2 --pedantic -Wall -Wextra -o demo undef_demo.c
./demo
