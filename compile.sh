#!/bin/bash

#http://phoxis.org/2009/12/01/beginners-guide-to-gcc/

BIN_DIR=bin/

FILES="src/assembler.c src/memory.c src/cpu.c src/zetaTools.c"
EXE=zeta80.o

switch=0

END=""

for var in "$@"
do
	if [ ${var:-0} == 'help' -o ${var-0} == '-h' ] ;
		then
		switch=1
	elif [ ${var:-0} == '-warn' ] ; 
		then
		END="$END -Wall -Wextra -Werror"
	elif [ ${var:-0} == '-debug' ] ;
		then
		END="$END -g"
	elif [ ${var:-0} == '-noassemble' ] ;
		then
		FILES="src/main.c src/memory.c src/cpu.c src/zetaTools.c"
	fi
done

STRING="gcc $FILES -o $BIN_DIR$EXE$END -lncurses"


if [ $switch == 1 ];
then 
	STRING="compile.sh <args>"
	STRING="$STRING\n-warn		Compile with warnings";
	STRING="$STRING\n-debug		Add Debug Flag"
	STRING="$STRING\n-noassemble		Do not compile with in built assembler"
	STRING="$STRING\n-help		Show this help menu"
	echo $STRING
fi

if [ $switch == 0 ];
	then 
	echo "Removing old binaries..."
	rm -r bin/*
	echo "Compiling: $STRING...."
fi

$STRING

echo "Done."