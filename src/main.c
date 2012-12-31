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
#include "assembler.h"
#include "cpu.h"

int verbose = 0;

int main(int argc, char *argv[]){
	printf("--z80 cpu and memory emulation--\n\n" );
	char* rom = "z801.eprom";
	FILE *file;
	int y;
	int switter =0;

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
			else if(strcmp(argv[y], "-a") == 0){
				if(y+1 < argc){
					y++;
					file = fopen( argv[y], "r" );
					switter=1;
					if(file == 0){
						printf("ERROR: Could not read file %s", argv[y]);
						return 0;
					}
				}
				else {
					printf("ERROR: Please set -a value");
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


	if(verbose > 1){
		debugRegister();
	}

	if(switter ==1){
		char l;
		int x =80;
		printf("Assembly file included, reading file...\n");
		char base[x];
		for(y = 0; y < x ; y++){
			base[y] =' ';
		}
		char com[x];
		char* baseptr = &base[0];
		char* comptr = &com[0];
		memcpy(comptr, baseptr, x);
		
		int comcount =0;
		int opt = 1;
		do{
			l=fgetc(file);
			if(l == ';'){
				opt = 0;
			}
			else if(l == '\n' || l == EOF){
				opt = 1;
				parseCommand2(comptr, x, verbose);
				memcpy(comptr, baseptr, x);
				comcount=0;
			}
			else{
				if(opt == 1){
					if(comcount > x-1){
						printf("An error, has occured, %s does not support assembly greater than %d chars long\n", argv[1],x);
					}
					else if(l != '\n'){
						com[comcount] = l;
						comcount++;
					}
				}
			}
		}
		while(l != EOF);
		fclose( file );
		saveEPROM();
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
	printf("zeta80.o <args>\n");
	printf("-r		Location to store ROM data\n");
	printf("-a		Assembly file, to load in ROM (WARN: will overwrite current ROM)\n");
	printf("-v		Verbose output on\n");
	//printf("-vv	Very verbose output on\n");
	//printf("-d 	Set RAM in bytes (ie -d 32000 for 32kb) ");
	//printf("-e 		Set ROM in bytes (ie -d 32000 for 32kb) ");
}

