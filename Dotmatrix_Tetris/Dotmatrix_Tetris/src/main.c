/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ledmatrix.c
**
** Beschrijving:	Simple HT16K33 Ledmatix demo.
** Target:			AVR mcu
** Build:			avr-gcc -std=c99 -Wall -O3 -mmcu=atmega128 -D F_CPU=8000000UL -c ledmatrix.c
**					avr-gcc -g -mmcu=atmega128 -o ledmatrix.elf ledmatrix.o
**					avr-objcopy -O ihex ledmatrix.elf ledmatrix.hex 
**					or type 'make'
** Program:			avrdude.exe" -c usbasp -p m168 -U flash:w:LedMatrixToBb.hex
** Author: 			dkroeske@gmail.com
** -------------------------------------------------------------------------*/

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "main.h"
#include "display.h"

unsigned char display_array[9][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1}
};

struct blockLocation {
	int  row;
	int  column;

}blockLocation;
/******************************************************************/
void wait( int ms )
/* 
short:			Busy wait number of millisecs
inputs:			int ms (Number of millisecs to busy wait)
outputs:	
notes:			Busy wait, not very accurate. Make sure (external)
				clock value is set. This is used by _delay_ms inside
				util/delay.h
Version :    	DMK, Initial code
*******************************************************************/
{
	for (int i=0; i<ms; i++)
	{
		_delay_ms( 1 );		// library function (max 30 ms at 8MHz)
	}
}

void setupDisplayArray(unsigned char* displayBuffer){
	int row;
	for(row = 0; row < 8; row++) {
		int col;
		unsigned char tempRow = 0b00000000 | display_array[row][7];
		for(col = 0; col < 8; col++) {
			if (row == blockLocation.row || row == blockLocation.row - 1) {
				if(col == blockLocation.column || col == blockLocation.column + 1) {
					tempRow = tempRow | (1 << col);
				}
			}
			tempRow = tempRow | ((display_array[row][7 - col]) << col);
		}
		displayBuffer[row] = tempRow; 
	}
}
void startGame(){
	blockLocation.row = 1;
	blockLocation.column = 3;
	animateGame();
}

void animateGame() {
	unsigned char displayBuffer[8];
	while(1){
		setupDisplayArray(displayBuffer);
		drawArray(displayBuffer);
		wait(2000);
		blockLocation.row++;
	}
}

/******************************************************************/
int main( void )
/* 
short:			main() loop, entry point of executable
inputs:			
outputs:	
notes:			Looping forever, trashing the HT16K33
Version :    	DMK, Initial code
*******************************************************************/
{
	displayInit();
	startGame();
	wait(500);

	//displayChar('1', 0, 0);
	//display();
	while(1==1) {
	}

	return 1;
}

