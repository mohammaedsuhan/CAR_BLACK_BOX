/*
 * File:  uart.h
 * Author: MOHAMMAED SUHAN
 * project :Car black box
 *
 * Created on 24 November, 2024, 9:15 AM
 */
#ifndef SCI_H
#define SCI_H

#define RX_PIN					TRISC7
#define TX_PIN					TRISC6

//all uart function prototypes

void init_uart(void);

void putch(unsigned char byte);

int puts(const char *s);

unsigned char getch(void);

unsigned char getch_with_timeout(unsigned short max_time);

unsigned char getche(void);

#endif
