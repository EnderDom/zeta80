#include <stdio.h>
#include <string.h>
#include "assembler.h"
#include "memory.h"
#include "zetaTools.h"

int parseCommand(char* com, int x, int v){
	verbosity = v;
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

int parseCommand2(char* com, int x, int v){
	verbosity = v;
	int i =0;
	int l=0;
	printf("LEN: %d LINE: %s", x,com);
	for(i =x ; i>-1;i--){
		if(com[i] == '\n'){
			
		}
		else if(com[i] != ' '){
			l=i;
			break;
		}
	}
	if(i > 0){
		char opcode[l+1];
		for(; i > -1;i--){
			opcode[i] = com[i];
		}
		opcode[l+1] =0;
		char c[] = "000";
		c[2] = 0;
		char* carr = &c[0];
		int j =0;
		for(i =0; i < 256; i++){
			if(strcmp(table1[i], opcode) == 0){
				j =i;
				carr = getHexFromInt(j,1, carr);
				printf("OPCODE %s, is equal to %s\n", opcode, c);
				break;
			}
		}
		if(i == 256)printf("OPCODE %s, not found\n", opcode);
	}
	else{
		if(verbosity > 1){
			printf("Empty line , skipping...\n");
		}
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
	}
	
	// ____ DEBUG ___
	//Shift 3 for hex debug output
	unsigned long int op = opcode;
	unsigned long int *opptr = &op;
	
	if(verbosity >1){
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
		if(verbosity >1){
			printf("Wrote opcode to memory at byte numb %lu\n", assemblerloc);
			printf("Wrote opcode to long array at index %d, this was shifted by %d and or'd \n", arrpoint, w);
			printf("This changed %lu to %lu\n", t, eprom[arrpoint]); 
		}
		assemblerloc++;
	}
    
    return 0;
}