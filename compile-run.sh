#!/bin/bash -eu
cd "$(dirname ${BASH_SOURCE[0]})"
gcc ./main.c -Wall -std=c99
exec ./a.out $@

