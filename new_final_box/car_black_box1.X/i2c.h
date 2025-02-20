/*
 * File:  i2c.h
 * Author: MOHAMMAED SUHAN
 * project :Car black box
 *
 * Created on 24 November, 2024, 9:15 AM
 */
#ifndef I2C_H
#define I2C_H


void init_i2c(void);
void i2c_start(void);
void i2c_rep_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(void);

#endif