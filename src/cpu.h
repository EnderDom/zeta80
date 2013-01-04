#ifndef CPU_H
#define CPU_H

/*
 * For the cpu flag, I'm making the assumption it the flags are always 0-1 and thus only need a 8bit register
 * For simplicity I'm using a 16bit data value for the double registers, hopefully this shouldn't cause
 * to many problems :S
 */
//			           0  1        2    3    4    5    6   7  8       9    10   11   12
//REGISTERS		           A  CZPSNH   BC   DE   HL   IX   IY  A' CZPSNH' BC'  DE'  HL'  SP
unsigned short int reg[13];
static const unsigned short int size[13]   = {2, 2,      4,  4,  4,  4,  4,  2, 2,     4,  4,  4,   2};
static const unsigned short int spacing[13] = {1, 5,       1,   1,  1,   1,   1,  1, 6,      1,   1,   1,   1};


int startProcessor();
int debugRegister();
#endif

#define regSize 13
