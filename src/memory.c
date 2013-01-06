#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "zetaTools.h"

/**
@param the size of disks to be created in bytes 
*/
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

unsigned long int getDiskSize(){
	return disksize;
}

/**
* Will return value x, an unsigned long int,
* where 0 < x < 256
* This represents the byte at position called
* 
* position should be 
* 
* @param position integer which should represent
* the position in memory to retrieve byte data from
* 0 < position < disksize*2
*/
unsigned long int getByte(unsigned int position){
	if(position > disksize*2){
		printf("(zeta80)ERROR: OUT OF MEMORY, ROM size is set to %lu, attempting to assemble opcode to outside that at %d bytes", disksize, position);
	}
	return position > disksize ? 0xFF&(ram[position/bytesinlong]>>position%bytesinlong): 0xFF&(eprom[position/bytesinlong]>>position%bytesinlong);
}

/**
* @param bytes : A pointer to the first/only long int to add data from
* @param numberofbytes : the number of bytes to be written to memory, if the number 
* is greater than the bytesinlong variable, bytes is assumed to be an array of 
* length numberofbytes/bytesinlong +1
* @param position an unsigned integer representing
* the write as byte number, ignoring the internal storage mechanisms
* 
*/
void setBytes(unsigned long int* bytes, int numberofbytes, unsigned int byteposition){
	if((byteposition+numberofbytes) > disksize*2){
		printf("(zeta80)ERROR: OUT OF MEMORY, ROM size is set to %lu, attempting to assemble opcode to outside that at %d bytes", disksize, byteposition);
	}
	
	//Position within the internal data structure
	unsigned int position = byteposition/bytesinlong;
	//Offset
	int offset = byteposition%bytesinlong;
	printf("%d %d \n", position, offset);
	
	//Masks
	unsigned long int leftmask = -1;
	unsigned long int rightmask = -1;
	
	//Number of _bits_ in a long type
	int len = bytesinlong<<3;
	//Offset in bits
	offset<<=3;

	//Loop Iteration values
	int i =0;
	int inter = (numberofbytes/bytesinlong)+1;
	
	for(i =0; i < inter; i++){
		//Next integer if exist
		if(i != 0)bytes++;
		//Recalculate width
		int width = i < inter-1 ? len : (numberofbytes - (i*bytesinlong))<<3;
		/*
		* Set all bits to 1
		*/
		leftmask = -1;
		rightmask = -1;

		/*
		* ternary is due to annoying thing were
		* if you shift by equal or more than the size
		* of the data type it doesn't play ball.
		* 
		* There's likely an alternative way of doing
		* this but I can't think of it right now
		
		*/ 		
		leftmask = (len-offset) >= len ? 0: leftmask>>(len-offset);//Left
		rightmask = (offset+width) >= len ? 0 : rightmask <<(offset+width);//Right
		leftmask |= rightmask;//Merge
		
		if((position+i) < disksize){
			eprom[position+i] &= leftmask;//Clean
			eprom[position+i] |= (((*bytes)<<offset)&~leftmask);//Paste (&clean data)
		}
		else{
			ram[position+i] &= leftmask;//Clean
			ram[position+i] |= (((*bytes)<<offset)&~leftmask);//Paste (&clean data)
		}

		/*
		* Need to add the rest of data which doesn't fit
		* into previous index due to offset
		*/		
		if(width+offset > len){			
			/* No leftmask actually needed
			* as data will always be stored at first 
			* byte position
			*/
			//Right
			rightmask = -1;
			rightmask <<= offset-width;
			//Cross fingers bother numberofbytes and disksize are correct
			if((position+i) < disksize){
				eprom[position+i+1] &=rightmask;//Clean
				eprom[position+i+1] |=((*bytes)>>((len-offset)&~rightmask));//Paste (&clean data)
			}
			else{
				ram[position+i+1-disksize] &=rightmask;//Clean
				ram[position+i+1-disksize] |=((*bytes)>>((len-offset)&~rightmask));//Paste (&clean data)
			}
		}
	}
	return;
}

