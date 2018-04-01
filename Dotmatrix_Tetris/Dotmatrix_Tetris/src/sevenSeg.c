#define BASEDATACMD  0x40	//std write    off 0x01 for read
#define FIRSTSEGMENT 0xC0	//first segment off it with a counter value for the next segment
#define DISPLAY_ON	0x88	//off this with duty cycle pulse for brightness
#define DISPLAY_OFF 0x80
#define MAXDIGITS 4
#define F_CPU 8000000

#define segOne 0
#define segTwo 1
#define segThree 2
#define segFour 3


#include <avr/io.h>
#include <util/delay.h>
#include "sevenSeg.h"

static const uint8_t _digitInSegments[] =
{
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x6F  // 9
};

void sevenSegInit(void){
	/*
	short:			init the seven segment
	inputs:
	outputs:
	notes:			cleans pins and de sevensegments
	Version :    	Rick Verstraten, Lars Moesman
	*******************************************************************/
	DDRB = 0x03;	//pd0 and pd1 are output
	PORTB = 0x00;	//all pins are low
	sevenSegCleanup();
}
 
void showDigit(int digit){
/*
	short:			displays entert digit.
	inputs:
	outputs:
	notes:			if digit > 9999 it mods it; so 1000 becomes 0;
	Version :    	Rick Verstraten, Lars Moesman
	*******************************************************************/
	uint8_t segments, i;
	for(i = 1; i <= MAXDIGITS; i ++){	//start at 1 because you count from 0
		segments = digit % 10;
		sevenSegDisplaySegs(MAXDIGITS - i,_digitInSegments[segments]);
		digit /= 10;
	}
}
void sevenSegCleanup(void){
/*
	short:			writes zero to all segnments
	inputs:
	outputs:
	notes:			
	Version :    	Rick Verstraten, Lars Moesman
	*******************************************************************/
	sevenSegDisplaySegs(segOne ,0x00);	
	sevenSegDisplaySegs(segTwo ,0x00);
	sevenSegDisplaySegs(segThree ,0x00);
	sevenSegDisplaySegs(segFour ,0x00);
}

void sevenSegConf(void){
/*
	short:			sevenseg configuration
	inputs:
	outputs:
	notes:			turns on display and set brighness hardcoded to 0x03, could make input for this.
	Version :    	Rick Verstraten, Lars Moesman
	*******************************************************************/
	uint8_t command;
	command = DISPLAY_ON;
	command |= 0x03;	//brightness
	sevenSegCmd(command);
}
void sevenSegDisplaySegs(uint8_t segAddress, uint8_t value){
/*
	short:			displays segments
	inputs:
	outputs:
	notes:			turns on the leds given in the value field, in the 7seg of segAddress
	Version :    	Rick Verstraten, Lars Moesman
	*******************************************************************/
	sevenSegCmd(BASEDATACMD | 0x04);	//fixed addresses
	sevenSegStart();
	sevenSegWriteByte(FIRSTSEGMENT | segAddress);	//address to write to
	sevenSegWriteByte(value);						//value to write
	sevenSegStop();
	sevenSegConf();
}
void sevenSegCmd(uint8_t cmd){
/*
	short:			sent command
	inputs:
	outputs:
	notes:			
	Version :    	Rick Verstraten, Lars Moesman
	*******************************************************************/
	sevenSegStart();
	sevenSegWriteByte(cmd);
	sevenSegStop();
}
void sevenSegWriteByte(uint8_t byteval){
/*
	short:			write byte
	inputs:
	outputs:
	notes:			for debug purposes you could return the ack field, this value is 0 if chip acknowledge
	Version :    	Rick Verstraten, Lars Moesman
	*******************************************************************/
	uint8_t ack;
	int i;
	for(i = 0; i < 8; i++){			//writes the data
		PORTB &= ~0x02;	//clk low
		wait_us(50);
		if(0x01 & byteval){	// dio should send high
			PORTB |= 0x01;
			}else{				// dio should send low
			PORTB &= ~0x01;
		}
		PORTB |= 0x02;	//clk high
		wait_us(50);
		byteval = byteval >> 1;
	}
	//ack
	PORTB &= ~0x02;	//clk low
	DDRB &=  ~0x01; //dio is input
	PORTB |=  0x01; //dio is high, ack should pull it low
	wait_us(50);
	
	if((PINB | 0x01) > 0){
		ack = 1;
		}else{
		ack = 0;
	}
	if(1 == ack){	//if not ack pull DIO down yourself
		DDRB |= 0x01;
		PORTB &= ~0x01;
	}
	wait_us(50);
	PORTB |= 0x02;	//clk high   falling edge 9th clock
	wait_us(50);
	PORTB &= ~0x02;	//clk low	properly ends the ack
	DDRD |= 0x01;
	
}

void sevenSegStart(void){
	/*
	short:			send start pulse
	inputs:
	outputs:
	notes:			not this is not default i2c
	Version :    	Rick Verstraten, Lars Moesman
	*******************************************************************/
	PORTB |= 0x01;	//DIO pin high	others left alone
	PORTB |= 0x02;	//clk pin high
	wait_us(50);
	PORTB &= ~0x01;	// DIO  pin low
}

void sevenSegStop(void){
		/*
	short:			send stop pulse
	inputs:
	outputs:
	notes:			not this is not default i2c
	Version :    	Rick Verstraten, Lars Moesman
	*******************************************************************/
	PORTB &= ~0x02; // CLK pin low	others left alone
	wait_us(50);
	PORTB &= ~0x01;	// DIO  pin low
	wait_us(50);
	PORTB |= 0x02;	//CLK high
	wait_us(50);
	PORTB |= 0x01;	//DIO high
}

void wait_us( int us ) {
/*
	short:			wait in microseconds
	inputs:
	outputs:
	notes:			
	Version :    	Rick Verstraten, Lars Moesman
	*******************************************************************/
	for (int i=0; i<us; i++)
	{
		_delay_us( 1 );
	}
}