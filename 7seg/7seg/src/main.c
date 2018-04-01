/*
 * 7seg.c
 *
 * Created: 31-3-2018 14:04:12
 * Author : Rick
 */ 

#include <avr/io.h>
#include "sevenSeg.h"
#include <time.h>
time_t t = 0;
int main(void)
{
	sevenSegInit();
    /* Replace with your application code */
    while (1) 
    {
		t++;
		wait(100);
		showDigit(t);
    }
}

