#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "zetaTools.h"

int initialiseMemory(int disksize_, char* rom){
	romfile = rom;
	
	disksize = disksize_;//bytes
	/*
	 * In the event that long size changes in the future/past 
	 * versions of C, leave this in, else you can skip it ;) 
	*/
	diskarrsize = disksize/bytesinlong;
	printf("Compiler long int is %dbyte ROM & RAM set long arrays of %lu bytes size\n", bytesinlong, diskarrsize);		
	
	printf("Allocating Memory...\n");
	
	ram = malloc(disksize);
	eprom = malloc(disksize);
	return 0;
}

int freeMemory(){
	free(ram);
	free(eprom);
	return 0;
}

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
	printf("Saving ROM to %s...\n", romfile);
	FILE *file = fopen(romfile, "w+b");
	fwrite(eprom, bytesinlong, diskarrsize, file);
	fclose(file);
	return 0;
}

void setRomfile(char* filename){
	romfile = filename;
}

/*
* @param bytes : A pointer to the first/only long int to add data from
* @param numberofbytes : the number of bytes to be written to memory, if the number 
* is greater than the bytesinlong variable, bytes is assumed to be an array of 
* length numberofbytes/bytesinlong +1
* @param position : position to write as byte number, ignoring the internal storage mechanisms
*/
int setBytes(unsigned long int* bytes, int numberofbytes, unsigned int byteposition){
	if(byteposition >= disksize){
		printf("ERROR: OUT OF MEMORY, ROM size is set to %lu, attempting to assemble opcode to outside that at %d bytes", disksize, byteposition);
	}
	
	int position = byteposition/bytesinlong;
	int offset = byteposition%bytesinlong;
	printf("%d %d \n", position, offset);
	unsigned long int leftmask = -1;
	unsigned long int rightmask = -1;
	
	int len = bytesinlong*8;

	offset*=8;
	/*
	int i =0;
	int inter = (numberofbytes/bytesinlong)+1;
	
	
	for(i =0; i < inter; i++){
		leftmask = -1;
		rightmask =-1;
		int width = (numberofbytes-(i*8))*8;
		if(width+offset =< len){
		}
		else{
			->leftside
			if((len-offset) >=64) leftmask = 0;
			else leftmask >>=len-offset;
		
			if((offset+numbs) >= len) rightmask = 0;
			else rightmask <<= offset+numbs;
			
		}
	}
	*/
	
	
	int numbs = numberofbytes*8;
	char* c=NULL;
	if(numbs + offset < len){
		//Create mask to wipe current datas
		
		//Check to bypass the wierdness of shifting more than sizeof long in bits
		if((len-offset) >=64) leftmask = 0;
		else leftmask >>=len-offset;
		
		if((offset+numbs) >= len) rightmask = 0;
		else rightmask <<= offset+numbs;
		
		leftmask |= rightmask;
		/*
		* Write data
		*/
		printf("EROM: %s\n", getHexFromLong(eprom[position], c));
		eprom[position] &= leftmask;
		printf("MASK: %s\n", getHexFromLong(leftmask, c));
		printf("EROM: %s\n", getHexFromLong(eprom[position], c));
		eprom[position] |= (((*bytes)<<offset)&~leftmask);
		printf("SCOR: %s\n", getHexFromLong((*bytes), c));
		printf("SCOR: %s\n", getHexFromLong((((*bytes)<<offset)&~leftmask), c));
		printf("EROM: %s\n", getHexFromLong(eprom[position], c));
	}
	else{
		
	}
	return 0;
}


