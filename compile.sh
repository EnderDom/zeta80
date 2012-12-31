#!/bin/bash

#http://phoxis.org/2009/12/01/beginners-guide-to-gcc/

BIN_DIR=bin/

FILES="src/main.c src/memory.c src/assembler.c src/cpu.c src/zetaTools.c"
EXE=zeta80.o

switch=0

if [ ${1:-0} == 'help' -o ${1:-0} == '-h' ] ;
then
	$switch = 1
elif [ ${1:-0} == 'warn' ]  ;
then
	STRING="gcc $FILES -o $BIN_DIR$EXE -Wall -Wextra -Werror -g"
else
	STRING="gcc $FILES -o $BIN_DIR$EXE"
fi
echo "Compiling: $STRING...."

if [ $switch == 0 ];
then 
	$STRING
else
	echo "compile.sh <args>\ncompile warn - Compile with warnings\ncompile help - Show this help menu"
fi

echo "Done."