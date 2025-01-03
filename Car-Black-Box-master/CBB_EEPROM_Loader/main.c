/*
Name         : Prabhat Kiran
Date         : 14th December 2022
Description  : To initially Load the Default Password in the External EEPROM for the Car Black Box to access while Execution.
*/

#include <xc.h>
#include "ext_eeprom.h"
#include "i2c.h"

void main()
{
	init_i2c ();
	write_ext_eeprom (0xC8, '0');
	write_ext_eeprom (0xC9, '0');
	write_ext_eeprom (0xCA, '0');
	write_ext_eeprom (0xCB, '0');
}
