/*
Name         : Prabhat Kiran
Date         : 14th December 2022
Description  : To configure the Timer2.
*/

#include <xc.h>
#include "timer0.h"

void init_timer0 (void)
{
	/* Basic Configuration for Timer2 */
    TMR2ON = 1;
    PR2 = 250;
    TMR2IF = 0;
    TMR2IE = 1;
}
