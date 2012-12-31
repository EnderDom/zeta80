#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

int initialiseMemory(int disksize_, char* rom){
	romfile = rom;
	bytesinlong = sizeof(unsigned long int);
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

