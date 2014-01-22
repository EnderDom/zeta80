#include <stdio.h>
#include <string.h>
#include "assembler.h"
#include "memory.h"
#include "cpu.h"
#include "zetaTools.h"

//Recieves a assembly command, assumes
//command has no whitespace left of any coms
//whitespace right of coms is ignored
//lines with no coms are ignored
//Takes coms compares to the table
//and adds the command to memory starting
//at byte 0 in the eprom
int parseCommand(char* com, int x, int v){
	verbose = v;
	int i =0;
	printf("LEN: %d LINE: %s", x,com);
	int l=0;
	//Loops through command from end backwards
	for(i =x ; i>-1;i--){
		//If newline, which should be when i==x, do nothing
		if(com[i] == '\n');
		//Get the position of the first none whitespace character
		else if(com[i] != ' '){
			l=i+1;
			break;
		}
	}
	if(i > 0){//Acts only if none-whitespace chars
		int bytelen = 2;
		//Default char is
		unsigned long int opcode = 0xEDFF;

		char assemblya[l+1];
		for(; i > -1;i--){
			assemblya[i] = com[i];
		}
		assemblya[l]=0;
		//printf("OPCODE %s\n", assemblya);
		char* carr = NULL;
		
		for(i =0; i < 256; i++){
			if(strcmp(table1[i], assemblya) == 0){
				bytelen = 1;
				opcode = i;
				break;
			}
		}
		if(opcode == 0xEDFF){
			for(i =64; i < 128; i++){
				if(strcmp(table2[i-64], assemblya) == 0){
					bytelen = 2;
					opcode = 0xED00+i;
					break;
				}
			}
			for(i =160; i < 192; i++){
				if(strcmp(table2[i-96], assemblya) == 0){
					bytelen = 2;
					opcode = 0xED00+i;
					break;
				}
			}
		}
		
		if(opcode == 0xEDFF) printf("ERROR: OPCODE %s, is not recognised, check syntax (eg whitespace, len...)\n", assemblya);
		else{
			if(verbose>1) printf("OPCODE %s, is equal to %s\n", assemblya, getHexFromLong(opcode,carr));
			unsigned long int* opcodep = &opcode;
			setBytes(opcodep, bytelen, assemblerloc);
			assemblerloc += bytelen;
		}
	}
	else{
		if(verbose > 1)printf("Empty line , skipping...\n");
	}

	return 0;
}


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
	isLittleEndian();

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
				parseCommand(comptr, x, verbose);
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
	
	char* c = NULL;
	for(y =0 ; y < 40; y++){
		c= getHexFromLong(getByte(y) ,c);
		int  i =bytesinlong+2;
		while(i-- > 0){ 
			printf("%c", (*c));
			c++;
		}
		printf("\n");
	}
	
    
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
	printf("-a		Assembly file, to load in ROM (WARN: will overwrite current ROM)\n");
	printf("-v		Verbose output on\n");
	//printf("-vv	Very verbose output on\n");
	//printf("-d 	Set RAM in bytes (ie -d 32000 for 32kb) ");
	//printf("-e 		Set ROM in bytes (ie -d 32000 for 32kb) ");
}