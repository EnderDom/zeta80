#include <stdio.h>

char* getHexFromLong(unsigned long int i, char* arr){
	int size = sizeof(unsigned long int);
	unsigned long int mask = 15;
	unsigned long int a = 0;
	
	int j =0;
	char ar[(size*2)+3];
	ar[(size*2)+2] =0;
	for(j = 0; j < (size*2)+2;j++){
		ar[j] = '0';
	}
	ar[1] = 'x';
	for(j =(size*2)-1; j > -1;j--){
		a = mask & i;
		if(a < 10) ar[j+2] = (char)(((int)'0')+a);
		else ar[j+2] =(char)(((int)'A') + a-10);
		i>>=4;
	}
	arr = ar;
	return arr;
}

char* getHexFromShort(unsigned short int i, char* arr){
	int size = sizeof(unsigned short int);
	unsigned short int mask = 15;
	unsigned short int a = 0;
	
	int j =0;
	char ar[(size*2)+3];
	ar[(size*2)+2] =0;
	for(j = 0; j < (size*2)+2;j++){
		ar[j] = '0';
	}
	ar[1] = 'x';
	for(j =(size*2)-1; j > -1;j--){
		a = mask & i;
		if(a < 10) ar[j+2] = (char)(((int)'0')+a);
		else ar[j+2] =(char)(((int)'A') + a-10);
		i>>=4;
	}
	arr = ar;
	return arr;
}
