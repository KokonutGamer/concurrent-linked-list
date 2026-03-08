#!/bin/bash

# shortcut to compile and run the program

gcc -o unit_tests -g -Wall -Wextra -Wno-sign-compare hoh_linkedlist.c unit_tests.c
./unit_tests