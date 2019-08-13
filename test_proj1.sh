#!/bin/sh
set -eu
echo `clear`
gcc -Wall -W 2114033_2113043_proj1.c -o proj1 -lpthread  && ./proj1 100 1000
echo -e '\n'