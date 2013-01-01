#include <stdio.h>
#include <string.h>
#include "assembler.h"
#include "memory.h"
#include "cpu.h"
#include "zetaTools.h"

int parseCommand(char* com, int x, int v){
	verbose = v;
	int i =0;
	printf("LEN: %d LINE: %s", x,com);
	int l=0;
	for(i =x ; i>-1;i--){
		if(com[i] == '\n'){
			
		}
		else if(com[i] != ' '){
			l=i+1;
			break;
		}
	}
	if(i > 0){
		int bytelen = 2;
		unsigned long int opcode = 0xEDFF;

		char assemblya[l+1];
		for(; i > -1;i--){
			assemblya[i] = com[i];
		}
		assemblya[l]=0;
		printf("OPCODE %s\n", assemblya);
		char c[] = "000";
		c[2] = 0;
		char* carr = &c[0];
		
		for(i =0; i < 256; i++){
			printf("%d)%s compare with %s\n", i, table1[i], assemblya);
			if(strcmp(table1[i], assemblya) == 0){
				bytelen = 1;
				opcode = i;
				carr = getHexFromInt(opcode,1, carr);
				if(verbose==1) printf("OPCODE %s, is equal to %s\n", assemblya, c);
				break;
			}
		}
		if(opcode == 0xEDFF){
			for(i =64; i < 128; i++){
				printf("%d)%s compare with %s\n", i, table2[i-64], assemblya);
				if(strcmp(table2[i-64], assemblya) == 0){
					bytelen = 1;
					opcode = 0xED00;
					opcode += i;
					carr = getHexFromInt(opcode,1, carr);
					if(verbose==1) printf("OPCODE %s, is equal to %s\n", assemblya, c);
					break;
				}
			}
			for(i =160; i < 192; i++){
				printf("%d)%s compare with %s\n", i, table2[i-96], assemblya);
				if(strcmp(table2[i-96], assemblya) == 0){
					bytelen = 1;
					opcode = 0xED00;
					opcode += i;
					carr = getHexFromInt(opcode,1, carr);
					if(verbose==1) printf("OPCODE %s, is equal to %s\n", assemblya, c);
					break;
				}
			}
		}
		if(opcode == 0)printf("OPCODE %s, either NOP or not found\n", assemblya);
	}
	else{
		if(verbose > 1)printf("Empty line , skipping...\n");
	}
	return 0;
}

int debugOPCODE(unsigned long *i, int bits, char *arr){
    unsigned long int mask = 15;
    unsigned long a = 0;
    
    while(bits >-1){
	a = mask & (*i);
	//printf("%lu %lu %lu %d \n", mask, (*i),a, bits);
	if(a < 10){
	    *(arr+bits) = (char)(((int)'0')+a);
	}
	else{
	    *(arr+bits) = (char)(((int)'a') + a-10);
	}
	(*i)>>=4;
	bits--;
    }
    return 0;
}

int debugOPCODEbin(unsigned long *i, int bits, char *arr){
    unsigned long int mask = 1UL;   
    unsigned long a = 0UL;
   
    while(bits >-1){
	a = mask & (*i);
	//printf("%lu %lu %lu %d \n", mask, (*i),a, bits);
	if(a != 0){
	  *(arr+bits) = '1';
	}
	else{
	  *(arr+bits) = '0';
	}
	(*i)>>=1;
	bits--;
    }
    return 0;
}

int assembleOPCODE(unsigned long int *i){
	
	//Check for multi-byte opcodes
	int multi = bytesinlong-8;
	if(((*i) & (237UL<<multi)) == 0){//ED --> Extended instructions
		printf("Extended Instruction\n");
	}
	else if(((*i) & (203UL<<multi)) == 0){//CB --> Bit instructions
		printf("Extended Instruction\n");
	}
	else if(((*i) & (221UL<<multi)) == 0){//DD --> IX instructions
		printf("Extended Instruction\n");
	}
	else if(((*i) & (253UL<<multi))== 0){//FD --> IY instructions
		printf("Extended Instruction\n");
	}
	//Single byte opcodes
	else{
		if(assemblerloc == diskarrsize){
		      printf("ERROR: OUT OF MEMORY, ROM size is set to %lu, attempting to assemble opcode to outside that at %lu bytes", diskarrsize, assemblerloc);
		}
		int arrpoint = assemblerloc/bytesinlong;

		unsigned long int t = 0+eprom[arrpoint];
		int w = (bytesinlong*8)-((assemblerloc%bytesinlong)*8)-8;

		eprom[arrpoint] |= (*i)>>w;
		if(verbose >1){
			printf("Wrote opcode to memory at byte numb %lu\n", assemblerloc);
			printf("Wrote opcode to long array at index %d, this was shifted by %d and or'd \n", arrpoint, w);
			printf("This changed %lu to %lu\n", t, eprom[arrpoint]); 
		}
		assemblerloc++;
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