/*
Name         : Prabhat Kiran
Date         : 30th November 2022
Description  : To configure the CLCD and display the Data.
*/

#include <xc.h>
#include "clcd.h"

void clcd_write (unsigned char byte, unsigned char control_bit)
{
	CLCD_RS = control_bit;				//To decide if the Data or Instruction shall be passed from the Controller to the CLCD.
	CLCD_PORT = byte;					//To initialize the Data Lines with the 'byte' of Data received.

	/* Should be atleast 200ns as per the Datasheet */
	CLCD_EN = HI;
	CLCD_EN = LO;

	PORT_DIR = INPUT;					//To set the PORTD 7th bit as Input pin for checking if the CLCD is Busy or not.
	CLCD_RW = HI;						//To read the Data to the Controller from the CLCD.
	CLCD_RS = INSTRUCTION_COMMAND;		//To initalize that Instruction is being passed from Controller to CLCD.

	do
	{
		CLCD_EN = HI;					//To enable the CLCD.
		CLCD_EN = LO;					//To disable the CLCD.
	} while (CLCD_BUSY);				//To check if the CLCD is Busy or not by Polling the PORTD7 bit.

	CLCD_RW = LO;						//To write the Data from the Controller to the CLCD.
	PORT_DIR = OUTPUT;					//To set the PORTD 7th bit as Output pin.
}

void init_clcd (void)
{
	/* Set PORTD as output port for CLCD data */
	TRISD = 0x00;
	/* Set PORTC as output port for CLCD control */
	TRISC = TRISC & 0xF8;

	CLCD_RW = LO;

	CURSOR_HOME;
	TWO_LINE_5x8_MATRIX_8_BIT;
	DISP_ON_AND_CURSOR_OFF;
	CLEAR_DISP_SCREEN;
}

void clcd_print (const unsigned char *data, unsigned char addr)
{
	clcd_write (addr, INSTRUCTION_COMMAND);		//To fix the Address on the CLCD on which the Data shall be displayed.
	while (*data != '\0')						//The loop shall run till the Complete Data is sent to the CLCD.
	{
		clcd_write (*data++, DATA_COMMAND);		//To write the Data as one by one character on the CLCD.
	}
}

void clcd_putch (const unsigned char data, unsigned char addr)
{
	clcd_write (addr, INSTRUCTION_COMMAND);		//To fix the Address on the CLCD on which the Data shall be displayed.
	clcd_write (data, DATA_COMMAND);			//To write one character on the CLCD.
}
