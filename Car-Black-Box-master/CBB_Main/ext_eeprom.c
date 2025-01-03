/*
Name         : Prabhat Kiran
Date         : 14th December 2022
Description  : To configure the External EEPROM for Reading and Writing Data.
*/

#include "main.h"
#include "ext_eeprom.h"
#include <xc.h>
#include "i2c.h"

void write_ext_eeprom (unsigned char address, unsigned char data)
{
	i2c_start ();							//To initiate the START Condition.
	i2c_write (SLAVE_WRITE_EXT);			//To identify the SLAVE by writing it's Address on the BUS along with the R/W bit.
	i2c_write (address);					//To write the Memory Address of the SLAVE on which the Data shall be stored.
	i2c_write (data);						//To write the Data at the previously sent Memory Address.
	i2c_stop ();							//To initiate the STOP Condition.
	for (int wait = 50000;wait--;);			//To implement Blocking Delay.
}

unsigned char read_ext_eeprom (unsigned char address)
{
	unsigned char data;

	i2c_start ();							//To initiate the START Condition.
	i2c_write (SLAVE_WRITE_EXT);			//To identify the SLAVE by writing it's Address on the BUS along with the R/W bit.
	i2c_write (address);					//To write the Memory Address of the SLAVE from which the Data shall be read.
	i2c_rep_start ();						//To initiate the REPEAT START Condition.
	i2c_write (SLAVE_READ_EXT);				//To identify the SLAVE by writing it's Address on the BUS along with the R/W bit.
	data = i2c_read ();						//To read the Data from the previously sent Memory Address.
	i2c_stop ();							//To initiate the STOP Condition.

	return data;							//Return the 'data' read from the External EEPROM.
}
