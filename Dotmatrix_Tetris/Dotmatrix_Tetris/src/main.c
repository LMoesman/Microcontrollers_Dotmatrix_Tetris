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
time_t randomSeed = 0;	//is a time_t object because this is long.
int powerdOn = 0;
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
	int oneWidth;
}blockLocation;
int score = 0;
volatile int shouldReset = 0;

/******************************************************************/
void setupDisplayArray(unsigned char* displayBuffer){
	/*
	short:			setup DisplayArray
	inputs:			unsigned char array
	outputs:
	notes:			Makes an temp array to display on the dotmatrix
	Version :    	1.0
	Author	:		Lars Moesman & Rick Verstraten
	*******************************************************************/	
	int row;
	for(row = 0; row < 8; row++) {
		int col;
		unsigned char tempRow = 0b00000000 | display_array[row][7];
		for(col = 0; col < 8; col++) {
			if(!blockLocation.oneWidth){
				//Check if we are at the point where the block is
				if (row == blockLocation.row || row == blockLocation.row - 1) {
					if(col == blockLocation.column || col == blockLocation.column + 1) {
						//Draw the block in bites
						tempRow = tempRow | (128 >> col);
					}
				}	
			}else{
				//Check if we are at the point where the block is
				if (row == blockLocation.row || row == blockLocation.row - 1) {
					if(col == blockLocation.column) {
						//Draw the block in bites
						tempRow = tempRow | (128 >> col);
					}
				}
			}
			tempRow = tempRow | ((display_array[row][7 - col]) << col);
		}
		//Write part of display bites to displaybuffer
		displayBuffer[row] = tempRow; 
	}
}

/******************************************************************/
void startGame(){

/*	short:			starts the game
	inputs:			
	outputs:
	notes:			Starts the game by initing everything and call the animate method
	Version :    	1.0
	Author	:		Lars Moesman & Rick Verstraten
	*******************************************************************/		

	i = rand() % 7;
	if ((display_array[0][i] != 1) && (display_array[0][i+1] != 1)) { 
		//Init new block
		blockLocation.row = 0;
		blockLocation.column = i;
		blockLocation.oneWidth = rand() % 2;
		blockLocation.isAnimating = 1;
		animateGame();
	}else {
		//Game over
		//showDigit(9999);
		gameOver();
	}
}

/******************************************************************/
void resetGame(){
/*	short:			resets the game
	inputs:			
	outputs:
	notes:			Resets the game by setting variables to begin values
	Version :    	1.0
	Author	:		Lars Moesman & Rick Verstraten
	*******************************************************************/	

	//Reset score
	showDigit(0);
	int row;
	//Reset display array so it is empty again
	for (row = 0;row < 8;row++) {
			memcpy(display_array[row], (int[]){0,0,0,0,0,0,0,0}, 8);
	}
	//Reseting is over
	shouldReset = 0;
}

/******************************************************************/
void animateGame() {
/*	short:			animates the game
	inputs:			
	outputs:
	notes:			Controlling the falling of the blocks
	Version :    	1.0
	Author	:		Lars Moesman & Rick Verstraten
	*******************************************************************/	
	unsigned char displayBuffer[8];
	while(1){
		setupDisplayArray(displayBuffer);
		drawArray(displayBuffer);
		wait(2000);
		if(!blockLocation.oneWidth){
			//Check for collision under the block
			if (display_array[blockLocation.row+1][blockLocation.column] != 1 &&
		   	    display_array[blockLocation.row+1][blockLocation.column + 1] != 1) {
					blockLocation.row++;
			 }else {
				 break;
			 }
		}else{
			//Check for collision under the block
			if (display_array[blockLocation.row+1][blockLocation.column] != 1) {
				blockLocation.row++;
			}else {
				break;
			}
		}
	}
	//Block stopped falling so write to the display array for collision detection next iteration
	if(!blockLocation.oneWidth){
		display_array[blockLocation.row][blockLocation.column] = 1;
		display_array[blockLocation.row - 1][blockLocation.column] = 1;
		display_array[blockLocation.row][blockLocation.column + 1] = 1;
		display_array[blockLocation.row - 1][blockLocation.column + 1] = 1;
		blockLocation.isAnimating = 0;
	}else{
		display_array[blockLocation.row][blockLocation.column] = 1;
		display_array[blockLocation.row - 1][blockLocation.column] = 1;	
		blockLocation.isAnimating = 0;
	}
	
	checkForFullRows();
}


/******************************************************************/
ISR(INT2_vect) {
	/*
	short:			ISR INT2
	inputs:
	outputs:
	notes:			Moves block to the left if no collision occurred
	Version :    	1.0
	Author	:		Lars Moesman & Rick Verstraten
	*******************************************************************/
	
	//Check if both buttons are pressed
	if((PIND & 0x0C) == 0x0C){
		powerdOn = 1;
		shouldReset = 1;
		return;
	}
	//Check if block hasn't reached a wall
	if(blockLocation.column > 0){
		//Check for collision on the side
		if(display_array[blockLocation.row][blockLocation.column-1] == 0 &&
		   display_array[blockLocation.row - 1][blockLocation.column-1] == 0) {
				blockLocation.column--;
		}
	}
}

/******************************************************************/
ISR(INT3_vect) {
	/*
	short:			ISR INT3
	inputs:
	outputs:
	notes:			Moves block to the right if no collision occurred
	Version :    	1.0
	Author	:		Lars Moesman & Rick Verstraten
	*******************************************************************/
	//Check if block hasn't reached a wall
	if((PIND & 0x0C) == 0x0C){
		powerdOn = 1;
		shouldReset = 1;
		return;
	}
	
	//Check for collision on the side
	if(!blockLocation.oneWidth){
		if(blockLocation.column < 6){
			if(display_array[blockLocation.row][blockLocation.column+2] == 0 &&
			display_array[blockLocation.row - 1][blockLocation.column+2] == 0) {
				blockLocation.column++;
			}
		}
	}else{
		if(blockLocation.column < 7){
			if(display_array[blockLocation.row][blockLocation.column+1] == 0 &&
   			   display_array[blockLocation.row - 1][blockLocation.column+1] == 0) {
					blockLocation.column++;
			}
		}
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
	
	displayInit();
	sevenSegInit();
	showDigit(score);
	wait(500);
	uint8_t displayinit[8] = {0,0,0,0,0,0,0,0};
	drawArray(displayinit);
	while(!powerdOn){
		randomSeed++;
		showDigit(randomSeed);
		wait(100);
	}
	srand(randomSeed);
	while(1==1) {
		if (blockLocation.isAnimating == 0) {
			if(shouldReset == 0) {
				startGame();
				wait(500);
			}else{
				resetGame();
				wait(1000);
			}
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
	uint8_t empty[8] = {0,0,0,0,0,0,0,0};
	memcpy(display_array[0],empty,sizeof(unsigned char) * 8);		//clear the upper row
	memcpy(display_array[1],empty,sizeof(unsigned char) * 8);
}


void gameOver(){
	uint8_t deadFace[8] = {0b00000000,
					   0b10100101,
					   0b01000010,
					   0b10100101,
					   0b00000000,
					   0b00111100,
					   0b01000010,
					   0b01000010};
	drawArray(deadFace);
	wait(4000);
	resetGame();
	
}