
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "Declatarions.h"  /* Declatations for the project */


int getsw()
{
	int shiftPORTD = PORTD >> 8; // switches are connected to bits 8 - 11
	shiftPORTD = shiftPORTD & 0xf; // last digit (4 bits)
	return shiftPORTD;
}
int getbtns()
{
	int shiftPORTD = PORTD >> 4;// buttons are connected to bits 5,6,7
	shiftPORTD = shiftPORTD & 0xe;
	int shiftPORTF = PORTF >> 1;
	shiftPORTF = shiftPORTF & 0x1;
	int btn_input = shiftPORTD | shiftPORTF;
	//shiftPORTD = shiftPORTD & 0xf;// comaring between 3 bits which has the sum 7
	
	return btn_input;
}