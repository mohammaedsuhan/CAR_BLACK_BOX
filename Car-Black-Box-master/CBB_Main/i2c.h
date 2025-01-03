#ifndef I2C_H
#define I2C_H

/* Function Declarations */
void i2c_write (unsigned char data);
void init_i2c (void);
void i2c_start (void);
void i2c_rep_start (void);
void i2c_stop (void);
unsigned char i2c_read (void);

#endif
