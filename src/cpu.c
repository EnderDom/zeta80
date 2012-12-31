
#include <stdio.h>
#include "cpu.h"
#include "zetaTools.h"

int initialiseCPU(){
	int i;
	for(i =0; i < regSize; i++){reg[i]=0;}
	return 0;
}

int startProcessor(){
      //int counter = 20000;
      //for(;;){
	 
	    
     // }
  
  return 0;
}


int debugRegister(){
	printf("Register Debug:\n");
	printf("A  CZPSNH  BC  DE  HL  IX  IY  A'  CZPSNH'  BC'  DE'  HL'  SP\n");
	int x,y,hex;
	for(x =0; x < regSize; x++){
		hex = size[x]>>3;
		char arr[hex];
		char *arr_ptr[hex];
		*arr_ptr = &arr[0];
		unsigned short *reg_ptr = &reg[x];
		getHex(reg_ptr, hex, *arr_ptr);
		for(y = 0; y < hex; y++){
			printf("%c", arr[y]);
		}
		for(; y < spacing[x];y++){
			printf(" ");
		}
		printf("  ");
	}
	printf("\n");
	return 0;
}
