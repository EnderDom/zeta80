#include <stdio.h>

/*
* bits = Number of characters to write
* *i to number to convert to hex -1
* array to hold hex number
*/
char* getHex(unsigned short *i, int bits, char *arr){
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
	return arr;
}

char* getHexFromInt(int i, int bits, char *arr){
	int mask = 15;
	int a = 0;
	
	while(bits >-1){
		a = mask & i;
		if(a < 10){
			*(arr+bits) = (char)(((int)'0')+a);
		}
		else{
			*(arr+bits) = (char)(((int)'a') + a-10);
		}
		i>>=4;
		bits--;
	}
	return arr;
}

/*
* In order to debug an annoying bug:
* From: http://stackoverflow.com/questions/4497680/c-strcmp-source-code
*/
int strcmp2(const char *s1, const char *s2){
  int ret = 0;
  int i=0;

  while (!(ret = *(unsigned char *) s1 - *(unsigned char *) s2) && *s2){
	++s1, ++s2;
	//printf("%d\n", i);
	i++;
   }
  if (ret < 0)

    ret = -1;
  else if (ret > 0)

    ret = 1 ;

  return ret;
}
