#ifndef EXT_EEPROM_H
#define EXT_EEPROM_H

/* MACRO for the Slave Address */
#define SLAVE_READ_EXT		0xA1
#define SLAVE_WRITE_EXT		0xA0

/* Function Declarations */
void write_ext_eeprom (unsigned char address, unsigned char data);
unsigned char read_ext_eeprom (unsigned char address);

#endif
