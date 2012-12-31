/*
* Standard Libraries
*/

/*
Defines
*/
#define regSize 13

#ifndef MAIN_H
#define MAIN_H
int debugRegister();
int getHex(unsigned short *i, int bits, char *arr);
int parseCommand(char* com, int x);
int debugCommand(char *com, char *dest, char *src);
int assembler(char *com, char *dest, char *src);
int debugOPCODE(unsigned long *i, int bits, char *arr);
int debugOPCODEbin(unsigned long *i, int bits, char *arr);
int assembleOPCODE(unsigned long int *i);
int loadEPROM();
int saveEPROM();
int startProcessor();
int main(int argc, char *argv[]);
void printHelp();
#endif