#!/bin/bash

flex see.l && bison see.y -d && gcc -o see see.tab.c lex.yy.c

exit $?

