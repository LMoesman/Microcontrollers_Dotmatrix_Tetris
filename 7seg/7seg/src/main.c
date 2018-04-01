/*
 * 7seg.c
 *
 * Created: 31-3-2018 14:04:12
 * Author : Rick
 */ 

#include <avr/io.h>
#include "sevenSeg.h"
int i;
int main(void)
{
	sevenSegInit();
    /* Replace with your application code */
	 i = 0;
    while (1) 
    {
		showDigit(i);
		i+=3;
    }
}

