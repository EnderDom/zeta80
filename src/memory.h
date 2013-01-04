#ifndef MEMORY_H
#define MEMORY_H
unsigned long int *ram; //32k RAM
unsigned long int *eprom; //32k EPROM
unsigned long int disksize;//bytes
unsigned long int diskarrsize; //This is reset in main()
char *romfile;
static const int bytesinlong = sizeof(unsigned long int);


int initialiseMemory(int disksize_, char* rom);
int freeMemory();
int loadEPROM();
int saveEPROM();
void setRomfile(char* filename);
int setBytes(unsigned long int* bytes, int numberofbytes, unsigned int byteposition);
#endif