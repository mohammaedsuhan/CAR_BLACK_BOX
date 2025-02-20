/*
 * File:   EEPROM.c
 * Author: MOHAMMAED SUHAN
 * project :Car black box
 *
 * Created on 22 November, 2024, 9:15 AM
 */
#include "i2c.h"
#include "external_EEPROM.h"


void write_EEPROM(unsigned char address, unsigned char data)
{
	i2c_start();
	i2c_write(SLAVE_WRITE_E);
	i2c_write(address);
	i2c_write(data);
	i2c_stop();
    for(int i=10000;i--;);
}

unsigned char read_EEPROM(unsigned char address)
{
	unsigned char data;

	i2c_start();
	i2c_write(SLAVE_WRITE_E);
	i2c_write(address);
	i2c_rep_start();
	i2c_write(SLAVE_READ_E);
	data = i2c_read();
	i2c_stop();

	return data;
}
