/*
Name         : Prabhat Kiran
Date         : 28th November 2022
Description  : To implement I2C Communication between the Microcontroller and external EEPROM.
*/

#include <xc.h>
#include "i2c.h"
#include "ext_eeprom.h"

void init_i2c (void)
{
	/* Set SCL and SDA pins as Inputs because the Bus Idle condition needs to be checked using these Pins */
	TRISC3 = 1;
	TRISC4 = 1;

	/* Set I2C master mode */
	SSPCON1 = 0x28;

	/* To set the Baud Rate for the Communication */
	SSPADD = 0x31;

	/* Use I2C levels, worked also with '0' */
	CKE = 0;
	
	/* Disable Slew rate control worked also with '0' */
	SMP = 1;
	
	/* Clear SSPIF Interrupt flag */
	SSPIF = 0;
	
	/* Clear Bus Collision flag */
	BCLIF = 0;
}

void i2c_idle (void)
{
	while (!SSPIF);		//To check if the BUS is Idle or not by Polling for the SSPIF flag.
	SSPIF = 0;
}

void i2c_ack (void)		//To check if the Acknowledge is received from the Slave or not.
{
	if (ACKSTAT)		//If the Acknowledge is not pulled LOW by the Slave, the Slave doesn't exists.
	{
		/* Do debug print here if required */
	}
}

void i2c_start (void)	//To initiate the START condition on SDA and SCL.
{
	SEN = 1;			//Set the Start Enable bit.
	i2c_idle();			//Check if the BUS is Idle or not.
}

void i2c_stop (void)	//To initiate the STOP condition on SDA and SCL.
{
	PEN = 1;			//Set the Stop Enable bit.
	i2c_idle();			//Check if the Bus is Idle or not.
}

void i2c_rep_start (void)	//To initiate the REPEAT START condition on SDA and SCL.
{
	RSEN = 1;				//Set the Repeat Start Enable bit.
	i2c_idle();				//Check if the Bus is Idle or not.
}

void i2c_write (unsigned char data)		//To write the Data on the BUS for the Slave to receive.
{
	SSPBUF = data;						//Initialize the 'data' on the SSPBUF register for transmitting it on the BUS.
	i2c_idle();							//Check if the BUS is Idle or not.
}

void i2c_rx_mode (void)		//To Enable the Read Mode.
{
	RCEN = 1;				//Set the Receive Enable bit.
	i2c_idle();				//Check if the BUS is Idle or not.
}

void i2c_no_ack (void)		//To initiate NO Acknowledge from the Master on receiving the Data from the Slave.
{
	ACKDT = 1;				//After receiving the Data, the Master will not send any Acknowledge to the Slave.
	ACKEN = 1;				//To initiate Acknowledge sequence on SDA and SCL pins and transmit ACKDT data bit.
}

unsigned char i2c_read (void)			//To read the Data from the BUS for the Master to receive.
{
	i2c_rx_mode();						//The function call to Enable the Read Mode for the Master.
	//i2c_no_ack();

	return SSPBUF;						//The read Data is stored on the SSPBUF.
}
