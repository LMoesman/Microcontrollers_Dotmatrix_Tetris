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

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <time.h>
#include <string.h>

int i = 0;
#include "main.h"
#include "display.h"
#include "sevenSeg.h"

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
	int isAnimating;
}blockLocation;
int score = 0;

/******************************************************************/

void setupDisplayArray(unsigned char* displayBuffer){
	int row;
	for(row = 0; row < 8; row++) {
		int col;
		unsigned char tempRow = 0b00000000 | display_array[row][7];
		for(col = 0; col < 8; col++) {
			if (row == blockLocation.row || row == blockLocation.row - 1) {
				if(col == blockLocation.column || col == blockLocation.column + 1) {
					tempRow = tempRow | (128 >> col);
				}
			}
			
			tempRow = tempRow | ((display_array[row][7 - col]) << col);
		}
		displayBuffer[row] = tempRow; 
	}
}
void startGame(){
	blockLocation.isAnimating = 1;
	if ((display_array[0][i] != 1) && (display_array[0][i+1] != 1)) { 
		blockLocation.row = 0;
		blockLocation.column = i;
		animateGame();
	}else {
		//Game over
		showDigit(9999);
	}
	i = rand() % 7;
}

void animateGame() {
	unsigned char displayBuffer[8];
	while(1){
		setupDisplayArray(displayBuffer);
		drawArray(displayBuffer);
		wait(2000);
		 if (display_array[blockLocation.row+1][blockLocation.column] != 1 && display_array[blockLocation.row+1][blockLocation.column + 1] != 1) {
				blockLocation.row++;
		 }else {
			 break;
		 }
		
	}
	display_array[blockLocation.row][blockLocation.column] = 1;
	display_array[blockLocation.row - 1][blockLocation.column] = 1;
	display_array[blockLocation.row][blockLocation.column + 1] = 1;
	display_array[blockLocation.row - 1][blockLocation.column + 1] = 1;
	blockLocation.isAnimating = 0;
	
	checkForFullRows();
}

/******************************************************************/
ISR(INT2_vect) {
	/*
	short:			ISR INT1
	inputs:
	outputs:
	notes:			Set PORTC
	Version :    	1.0
	Author	:		Lars Moesman & Rick Verstraten
	*******************************************************************/
	
	
	if(PIND == 0x06){
		//Reset Game
		return;
	}
	if(blockLocation.column < 7){
		blockLocation.column++;
	}
	
}

/******************************************************************/
ISR(INT3_vect) {
	/*
	short:			ISR INT2
	inputs:
	outputs:
	notes:			Set PORTC
	Version :    	1.0
	Author	:		Lars Moesman & Rick Verstraten
	*******************************************************************/
	
		if(PIND == 0x06){
			//Reset Game
			return;
		}
		if(blockLocation.column > 0){
			blockLocation.column--;
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
	DDRD = 0x0C;
	
	EICRA |= 0xF0;
	EIMSK |= 0x0C;
	
	sei();
	
	srand(2344);
	displayInit();
	sevenSegInit();
	showDigit(score);
	wait(500);
	
	//displayChar('1', 0, 0);
	//display();
	while(1==1) {
		if (blockLocation.isAnimating == 0) {
			startGame();
			wait(500);
		}
	}
	return 1;
}

void checkForFullRows(void){
	int tempscore = 0;
	int x;
	for(x = 0; x < 8; x ++){
		int y;
		int count = 0;
		for(y = 0; y < 8; y ++){
			if(1 == display_array[x][y] && 1 == display_array[x - 1][y] ){ count++; }
		}
		if(8 == count){	//whole row filled
			tempscore += 10;
			theCoolFullRowAnimation(x);
			shoveDown(x);
		}
	}
score += tempscore;
	showDigit(score);
}

void shoveDown(int x){
	for(i = 0; i<= (x -2); x -= 2){
		int rowToShove = x - 2;
		memcpy(display_array[x],display_array[rowToShove],sizeof(unsigned char) * 8);	//cpy the upper row to this row
		memcpy(display_array[x-1],display_array[rowToShove - 1],sizeof(unsigned char) * 8);	//cpy the upper row to this row		//thwo times this function because blocks are 2*2
	}
}