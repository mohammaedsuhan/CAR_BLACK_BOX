/*
 * File:   main.c
 * Author: MOHAMMAED SUHAN
 * project :Car black box
 *
 * Created on 24 November, 2024, 9:15 AM
 */

#include <xc.h>
#include "mkp.h"
#include "clcd.h"
#include "adc.h"
#include "box.h"
#include "main.h"
#include "uart.h"
#include "ds1307.h"
#include "i2c.h"

unsigned char key ;
unsigned long int menu_flag=0;


// Function to initialize various peripherals and modules
void init_confi(void)
{
    init_adc();                // Initialize the ADC module
    init_clcd();              // Initialize the character LCD display
    init_matrix_keypad();     // Initialize the matrix keypad interface
    init_uart();              // Initialize UART communication
    init_i2c();               // Initialize I2C communication protocol
    init_ds1307();            // Initialize the DS1307 RTC module
}

// Main function where program execution begins
void main(void)
{
    init_confi();             // Call initialization function to set up peripherals
    while(1)                  // Infinite loop to keep the program running
    { 
        key = read_switches(STATE_CHANGE);  // Read the status of switches/keys
        if(menu_flag == 0)    // Check if in dashboard state
        {
            dashboard();       // Call function to display the dashboard
        }
        else if(menu_flag == 1) // Check if in menu screen state
        {
            menu_screen();     // Call function to display the menu screen
        }
        else if(menu_flag == 2) // Check if in view log state
        {
            view_log();        // Call function to view logs recorded by the system
        }
        else if(menu_flag == 3) // Check if in clear log state
        {
            clear_log();       // Call function to clear logs from memory/storage
        }
        else if(menu_flag == 4) // Check if in download log state
        {
            download_log();    // Call function to download logs to an external device or storage
        }
        else if(menu_flag == 5) // Check if in set time state
        {
            set_time();        // Call function to set the current time on the RTC module
        }
    }
}