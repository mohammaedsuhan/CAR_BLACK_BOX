/*
Name         : Prabhat Kiran
Date         : 5th December 2022
Description  : To perform the operations when an Interrupt is requested.
*/

#include <xc.h>
#include "main.h"
#include "timer0.h"

/* Global Variable declarations */
unsigned short blink;
extern unsigned short wait_flag;
unsigned short wait = 60;

void interrupt isr (void)
{
	static unsigned short for_minute;
	static unsigned short count;

	if (TMR2IF)							//If the Timer2 Overflow Interrupt Flag Bit is set, Timer2 is requesting for an Interrupt.
	{
		if (count++ == 20000)			//The formula for the Timer2 yields the result as: 1 sec for every 20000 ticks.
		{
			blink++;					//Increment the 'blink' value.
			if (wait_flag == 1)			//If the 'wait_flag' is set; decrement the 'wait' value.
				wait--;
			count = 0;					//Reset the 'count' to 0.
		}
	}

	TMR2IF = 0;							//To clear the Timer2 Overflow Interrupt Flag Bit to avoid Recursive interrupts.
}
