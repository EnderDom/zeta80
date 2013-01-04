
#include <stdio.h>
#include "cpu.h"
#include "zetaTools.h"
#include <ncurses.h>

int initialiseCPU(){
	int i;
	for(i =0; i < regSize; i++){reg[i]=0;}
	return 0;
}

int startProcessor(){
	debugRegister();
	//Thank god there's a library that does this
	initscr();
	printw("zeta80 - A z80 emulator by EnderDom\n");
	printw("Press any key to terminate this application: ");
	refresh();
	getch();
      //int counter = 20000;
      //for(;;){
	 
	    
     // }
	endwin();
	debugRegister();
	return 0;
}


int debugRegister(){
	printf("Register Debug:\n");
	printf("A  CZPSNH BC   DE   HL   IX   IY   A' CZPSNH' BC'  DE'  HL'  SP\n");
	int x,y,z;
	z = (sizeof(unsigned short int)*2)+2;
	for(x =0; x < regSize; x++){
		char* arrs = NULL;
		arrs = getHexFromShort(reg[x], arrs);
		for(y = 0; y < size[x]; y++){
			printf("%c", arrs[z-size[x]]);
		}
		for(y=0; y < spacing[x];y++){
			printf(" ");
		}
	}
	printf("\n");
	return 0;
}
