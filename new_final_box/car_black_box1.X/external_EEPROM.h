/*
 * File:  external_eeprom.h
 * Author: MOHAMMAED SUAHN
 * project :Car black box
 *
 * Created on 24 November, 2024, 9:15 AM
 */

#ifndef EXT_EEPROM_H
#define	EXT_EEPROM_H

#include <xc.h> // include processor files - each processor file is guarded.
#define SLAVE_READ_E	0xA1
#define SLAVE_WRITE_E		0xA0
void write_EEPROM(unsigned char address, unsigned char data);
unsigned char read_EEPROM(unsigned char address);
#endif	

