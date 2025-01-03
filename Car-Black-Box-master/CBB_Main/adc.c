/*
Name         : Prabhat Kiran
Date         : 30th November 2022
Description  : To Read the value from ADC.
*/

#include <xc.h>
#include "adc.h"

void init_adc (void)
{
	/* Selecting right justified ADRES Registers order */
	ADFM = 1;

	/*
	 * Acqusition time selection bits
	 * Set for 8 Tad
	 */
	ACQT2 = 1;
	ACQT1 = 0;
	ACQT0 = 0;

	/*
	 * Selecting the conversion clock of Fosc / 32 -> 1.6usecs -> 1Tad
	 * Our device frequency is 20 MHz
	 */
	ADCS0 = 0;
	ADCS1 = 1;
	ADCS2 = 0;

	/* Stop the conversion to start with */
	GODONE = 0;

	/* Selecting the channel */
	CHS2 = 1;

	/* Voltage reference bit as VSS */
	VCFG1 = 0;
	/* Voltage reference bit as VDD */
	VCFG0 = 0;

	/* Just clearing the ADRESH & ADRESL registers, for time pass */
	ADRESH = 0;
	ADRESL = 0;

	/* Turn ON the ADC module */
	ADON = 1;
}

unsigned short read_adc (unsigned char channel)
{
	unsigned short reg_val;

	ADCON0 = (ADCON0 & 0xc3) | (channel << 2);		//To set the Analog Channel Select bits.
	/* Start the conversion */
	GO = 1;											//Set the GO bit as 1 to start the Conversion.
	while (GO);										//When the Conversion is completed, the GO bit is Clear.
	reg_val = (ADRESH << 8) | ADRESL;				//Read the 8-bits from the High A/D register and 2-bits from the Low A/D register.

	return reg_val;
}
