
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
