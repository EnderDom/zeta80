/*
	z80 cpu and memory emulation
	
	Dominic Wood 2012

*/

/*
*Std libs
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* Headers
*/
#include "main.h"

unsigned long int *ram; //32k RAM
unsigned long int *eprom; //32k EPROM
unsigned long int disksize = 32000;//bytes
unsigned long int diskarrsize = 1000; //This is reset in main()
char *romfile = "z80.eprom";
unsigned long int assemblerloc = 0;
int bytesinlong = 8;
int verbose = 0;

/*
 * For the cpu flag, I'm making the assumption it the flags are always 0-1 and thus only need a 8bit register
 * For simplicity I'm using a 16bit data value for the double registers, hopefully this shouldn't cause
 * to many problems :S
 */
//			           0  1        2    3    4    5    6   7  8       9    10   11   12
//REGISTERS		           A  CZPSNH   BC   DE   HL   IX   IY  A' CZPSNH' BC'  DE'  HL'  SP
unsigned short int reg[13]  = {0, 0,       0,   0,   0,   0,   0,  0, 0,      0,   0,   0,   0};
unsigned short int size[13] = {8, 8,      16,  16,  16,  16,  16,  8, 8,     16,  16,  16,   8};//Size in bits
unsigned short int spacing[13] = {1, 6,       2,   2,   2,   2,   2,  2, 7,      3,   3,   3,   2};


int debugRegister(){
  
	printf("Register Debug:\n");
	printf("A  CZPSNH  BC  DE  HL  IX  IY  A'  CZPSNH'  BC'  DE'  HL'  SP\n");
	int x,y,hex;
	for(x =0; x < regSize; x++){
		hex = size[x]>>3;
		char arr[hex];
		char *arr_ptr[hex];
		*arr_ptr = &arr[0];
		unsigned short *reg_ptr = &reg[x];
		getHex(reg_ptr, hex, *arr_ptr);
		for(y = 0; y < hex; y++){
			printf("%c", arr[y]);
		}
		for(; y < spacing[x];y++){
			printf(" ");
		}
		printf("  ");
	}
	printf("\n");
	return 0;
}


int getHex(unsigned short *i, int bits, char *arr){
	unsigned short mask = 15;
	unsigned short a = 0;
	
	while(bits >-1){
		a = mask & (*i);
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



int parseCommand(char* com, int x){
	int i = 0;
	
	int open =0;
	
	int comm_c = 0;
	int dest_c = 0;
	int src_c = 0;
	char comm[10];
	char dest[10];
	char src[10];
	int y;
	for(y =0; y < 10; y++){
		comm[y] = ' ';
		dest[y] = ' ';
		src[y] = ' ';
	}
	char *comm_p = &comm[0];
	char *dest_p = &dest[0];
	char *src_p = &src[0];
	while(i < x){
		if(com[i] == ';'){
			break;
		}
		else if(com[i] == ':' || open == 3){
			debugCommand(comm_p, dest_p, src_p);
			assembler(comm_p, dest_p, src_p);
			for(y =0; y < 10; y++){
				comm[y] = ' ';
				dest[y] = ' ';
				src[y] = ' ';
			}
			comm_c =0;
			dest_c = 0;
			src_c =0;
			open =0;
		}
		else if(com[i] != ' '){
			if(open == 2){
				while(com[i] != ' ' && com[i] != ';' && com[i] != ':'){
					src[src_c] = com[i];
					src_c++;	 
					i++;
				}
				open = 3;
			}
			if(open == 1){
				while(com[i] != ' ' && com[i] != ','){
					dest[dest_c] = com[i];
					dest_c++;	 
					i++;
				}
				open = 2;
			}
			if(open == 0){  
				while(com[i] != ' '){
					comm[comm_c] = com[i];
					comm_c++;	 
					i++;
				}
				open = 1;
			}
		}
		i++;
	}
	if(comm_c !=0){
		assembler(comm_p, dest_p, src_p);
	}
	return 0;
}


int debugCommand(char *com, char *dest, char *src){
	int y =0;
	printf("COM:");
	while(y < 10){
		printf("%c", *(com+y));
 		if(*(com+y) == ' '){
		    break;
		}
		y++;
	}
	y=0;
	printf(" DEST:");
	while(y < 10){
		if(*(dest+y) == ' '){
		  break;
		}
		printf("%c", *(dest+y));
		y++;
	}
	y=0;
	printf(" SRC:");
	while(y < 10){
		printf("%c", *(src+y));
		if(*(src+y) == ' '){
		  break;
		}
		y++;
	}
	printf("\n");
	return 0;
}


/*
 A = 111
 B = 000
 C = 001
 D = 010
 E = 011
 H = 100
 L = 101
 
 I've done the endianness is a bit wierd

 
 */
int assembler(char *com, char *dest, char *src){
	unsigned long int opcode = 0;
	int y=0;
	int longs = bytesinlong<<3;
	int magicknumb = (longs)-2;
	
	//COMMANDS 
	if((*(com) == 'L' && *(com+1) == 'D') || (*(com) == 'l' && *(com+1) == 'd')){
		opcode+=1;
		opcode<<= magicknumb ; //01 000 000
		//printf("Set OPCODE to: %lu\n",opcode);
	}
	else if((*(com) == 'H' && *(com+1) == 'A' && *(com+2) == 'L' && *(com+3) == 'T') 
		|| (*(com) == 'h' && *(com+1) == 'a' && *(com+2) == 'l' && *(com+3) == 't')){
	      opcode += 0x76;
	      opcode<<= longs-8;
	}
	
	//LOCATIONS
	char *loop;
	loop = dest;
	unsigned long int tmp = 0;
	for(y = 0; y < 2; y++){
	      if(y == 1){
		  loop = src;
	      }
	      if(*(loop+1) == ' '){//SINGLE registers
		  if(*(loop) == 'a' || *(loop) == 'A'){
		      tmp = 7; //111
		  }
		  if(*(loop) == 'b' || *(loop) == 'B'){
		      tmp = 0; //000
		  }
		  if(*(loop) == 'c' || *(loop) == 'C'){
		      tmp = 1; //001
		  }
		  if(*(loop) == 'd' || *(loop) == 'D'){
		      tmp = 2; //010
		  }
		  if(*(loop) == 'e' || *(loop) == 'E'){
		      tmp = 3; //011
		  }
		  if(*(loop) == 'h' || *(loop) == 'H'){
		      tmp = 4; //100
		  }
		  if(*(loop) == 'l' || *(loop) == 'L'){
		      tmp = 5; //101
		  }
	      }
	      tmp<<=(magicknumb-(3*(y+1)));
	      opcode |= tmp;
	      //printf("Set OPCODE to: %lu\n",opcode);
	}
	
	// ____ DEBUG ___
	//Shift 3 for hex debug output
	unsigned long int op = opcode;
	unsigned long int *opptr = &op;
	
	if(verbose >1){
		char arr[longs>>2]; 
		char *debug_ptr = &arr[0];
		printf("OPCODE: %lu [",op);
		debugOPCODE(opptr, (longs>>2)-1, debug_ptr);
		for(y = 0; y < (longs>>2); y++){
		     printf("%c", arr[y]);
		     if((y&1) ==1){
			  printf(" ");
		     }
		}
		//Use full bit display for binary output of opcode
		printf("] {   ");
		char arr2[longs]; 
		char *debug_ptr2 = &arr2[0];
		op = opcode;
		opptr = &op;
		debugOPCODEbin(opptr, longs-1, debug_ptr2);
		for(y = 0; y < longs; y++){
		     printf("%c", arr2[y]);
		     if((y+1)%4 ==0){
			  printf(" ");
		     }
		}
		printf("   }");
		printf("\n");
	}
	opptr = &opcode;
	assembleOPCODE(opptr);
	
	
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
	
    }
    else if(((*i) & (203UL<<multi)) == 0){//CB --> Bit instructions
	
    }
    else if(((*i) & (221UL<<multi)) == 0){//DD --> IX instructions
	
    }
    else if(((*i) & (253UL<<multi))== 0){//FD --> IY instructions
	
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

/*
 * ENDOFCOMMANDS 
 * 
*/

int loadEPROM(){
	FILE *file = fopen(romfile, "r+b");
	printf("Loading ROM (%s)...\n",romfile);
	if(file == 0){
		saveEPROM();
	}
	else{
		fread(eprom, bytesinlong, diskarrsize, file);
		fclose( file );
	}
	return 0;
}

int saveEPROM(){
	printf("Saving ROM...\n");
	FILE *file = fopen(romfile, "w+b");
	fwrite(eprom, bytesinlong, diskarrsize, file);
	fclose(file);
	return 0;
}

int startProcessor(){
      //int counter = 20000;
      //for(;;){
	 
	    
     // }
  
  return 0;
}


int main(int argc, char *argv[]){
	printf("--z80 cpu and memory emulation--\n\n" );
		
	FILE *file;
	int y;
	int switter =0;

	if(argc > 1){
		for(y=0; y < argc; y++){
			if(strcmp(argv[y], "-r") == 0){
				if(y+1 < argc){
					y++;
					romfile = argv[y];
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
	
	/*
	 * In the event that long size changes in the future/past 
	 * versions of C, leave this in, else you can skip it ;) 
	*/
	bytesinlong = sizeof(unsigned long int);
	diskarrsize = disksize/bytesinlong;
	
	printf("Compiler long int is %dbyte ROM & RAM set long arrays of %lu bytes size\n", bytesinlong, diskarrsize);
	
	printf("Allocating Memory...\n");
	
	ram = malloc(disksize);
	eprom = malloc(disksize);

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
				parseCommand(comptr, x);
				memcpy(comptr, baseptr, x);
				comcount=0;
			}
			else{
				if(opt == 1){
					if(comcount > x-1){
						printf("An error, has occured, %s does not support assembly greater than %d chars long\n", argv[1],x);
					}
					else{
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
	
	//loadEPROM();
	//startProcessor();
	printf("Processor not currently emulated\n" );
    
	if(verbose > 1){
		debugRegister();
	}
	
	//SAVEROM
	//saveEPROM();
	free(ram);
	free(eprom);
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

