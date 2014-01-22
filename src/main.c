/*
	z80 cpu and memory emulation
	
	Dominic Wood 2012

*/

/*
*Std libs
*/
#include <stdio.h>
#include <string.h>

/*
* Headers
*/
#include "main.h"
#include "memory.h"
#include "cpu.h"

int verbose = 0;

int main(int argc, char *argv[]){
	printf("--z80 cpu and memory emulation--\n\n" );
	char* rom = "z801.eprom";
	FILE *file;
	int y;

	//Parse Arguments
	if(argc > 1){
		for(y=0; y < argc; y++){
			if(strcmp(argv[y], "-r") == 0){
				if(y+1 < argc){
					y++;
					rom = argv[y];
				}
				else {
					printf("ERROR: Please set -r value");
				}
			}
			else if(strcmp(argv[y], "-h") == 0){
				printHelp();
				return 0;
			}
			else if(strcmp(argv[y], "-v") == 0){
				verbose = 2;
			}
			else if(strcmp(argv[y], "-vv") == 0){
				verbose = 3;
			}
		}
	}

	initialiseMemory(32000, rom);
	isLittleEndian();

	if(verbose > 1){
		debugRegister();
	}
	
	loadEPROM();
	startProcessor();
	printf("Processor not currently emulated\n" );
    
	if(verbose > 1){
		debugRegister();
	}
	
	//SAVEROM
	saveEPROM();
	freeMemory();
	
	printf("Program Complete\n");
	return 0;
}

void printHelp(){
	printf("--zeta80 Help Menu--\n");
	printf("BINFILE <args>\n");
	printf("-r		Location to store ROM data\n");
	printf("-v		Verbose output on\n");
	//printf("-vv	Very verbose output on\n");
	//printf("-d 	Set RAM in bytes (ie -d 32000 for 32kb) ");
	//printf("-e 		Set ROM in bytes (ie -d 32000 for 32kb) ");
}

