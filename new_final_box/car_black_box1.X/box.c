/*
 * File:   car_black_box.c
 * Author: MOHAMMAED SUHAN
 * project :Car black box
 *
 * Created on 24 November, 2024, 9:15 AM
 */


#include <xc.h>
#include "mkp.h"
#include "main.h"
#include "clcd.h"
#include "adc.h"
#include "box.h"
#include "uart.h"
#include "ds1307.h"
#include "i2c.h"
#include "external_EEPROM.h"

void display_time(void);
unsigned char event[9][3] = {
                             "ON",
                             "GN",
                             "G1",
                             "G2",
                             "G3",
                             "G4",
                             "G5",
                             "GR",
                             "C "
                                },
              opt[4][16] = {
                            "view log       ",
                            "clear log      ",
                            "Download log   ",
                            "Set Time       "
                                             },
                  data[10][13],
                  time[10],
                  sub_state = 0,
                  clock_reg[3],
                  stored_event[10][17];
unsigned long int speed=0,
                  event_index1=0,
                  event_count=0,
                  view_flag=0,
                  address = 0x00,
                  count = 0,flag=0,count1=0,
                  hour,min,sec;
extern unsigned char key;
extern unsigned long int menu_flag;
static unsigned long int delay = 0;

void dashboard(void)
{
   
    clcd_print("TIME      EV  SP", LINE1(0));  // Display the header for the dashboard on the first line of the CLCD
    clcd_print(time, LINE2(0));               // Display the current time on the second line of the CLCD
    key = read_switches(STATE_CHANGE);        // Read the keypad for any key press
    if (key == 1)                             // Check if the 'Key 1' is pressed to indicate an event (Car Crash "C ")
    {
        event_index1 = 8;                     // Set the event index to "C " (Car Crash)
        store_event(event_index1);            // Store the event in the log
    }
    else if (event_index1 == 8 && (key == 2 || key == 3))  // If the previous event is "C " and 'Key 2' or 'Key 3' is pressed, reset to "ON"
    {
        event_index1 = 1;                        // Reset event index to "ON"
        store_event(event_index1);               // Store the event in the log
    }
    else if (key == 2)                           // Increment the event index if 'Key 2' is pressed, up to "G5"
    {
        if (event_index1 < 7)
        {
            event_index1++;                      // Increment event index
        }
        store_event(event_index1);                // Store the event in the log
    }
    else if (key == 3)
    {
        if (event_index1 > 1)
        {
            event_index1--;                                 // Decrement event index
        }
        store_event(event_index1);                          // Store the event in the log
    }
    clcd_print(time, LINE2(0));                            // Display the time
    clcd_print("  ", LINE2(8));
    clcd_print(event[event_index1], LINE2(10));           // Display the current event
    clcd_print("  ", LINE2(12));
    speed = read_adc(CHANNEL4) / 10.23;                    // Read the speed from the ADC (Analog-to-Digital Converter) and convert it to 0-99 scale
    if (speed > 99)
    {
        speed = 99;                                       // Limit the speed display to 99
    }
    clcd_putch(speed % 10 + '0', LINE2(14));   // Display the units digit
    clcd_putch(speed / 10 + '0', LINE2(15));   // Display the tens digit
    get_time();                                  // Get the current time from the RTC (Real-Time Clock)
    display_time();                                // Update the time on the CLCD
    if (key == 11 && menu_flag == 0)    // If 'Key 11' is pressed, activate the menu screen
    {
        menu_flag = 1;                 // Set menu flag to enable the menu screen
    }
}

void store_event(int event_index2)
{
    event_count++;                          // Increment the event count to track the total number of events logged
    for (int i = 0; i < 8; i++)            // Store the current time (8 characters) into the EEPROM, starting from the current address
    {
        write_EEPROM(address++, time[i]);  // Write each character of the time to EEPROM
    }
    for (int j = 0; j < 2; j++)           // Store the event code (2 characters) into the EEPROM
    {
        write_EEPROM(address++, event[event_index2][j]); // Write each character of the event to EEPROM
    }
    write_EEPROM(address++, speed % 10 + '0'); // Write the units digit of speed to EEPROM
    write_EEPROM(address++, speed / 10 + '0'); // Write the tens digit of speed to EEPROM
    if (event_count % 10 == 0) // If 10 events have been logged, reset the EEPROM address to 0 for circular storage
    {
        address = 0; // Reset address to the beginning of EEPROM
    }
}


void display(void)
{
    // Display the current menu options on the CLCD screen
    clcd_print(opt[count], LINE1(2));      // Print the current option at the first line of the display
    clcd_print(opt[count + 1], LINE2(2)); // Print the next option at the second line of the display
    if (flag == 0) 
    {
        clcd_print("->", LINE1(0));       // Print the arrow next to the first option
        clcd_print("  ", LINE2(0));       // Clear any arrow next to the second option
    }
    else 
    {
        clcd_print("  ", LINE1(0));       // Clear any arrow next to the first option
        clcd_print("->", LINE2(0));       // Print the arrow next to the second option
    }
}

void menu_screen(void)
{
    delay = 0;  // Initialize delay to 0 for menu navigation timing
    if (key == 12 && menu_flag == 1) // If the "down" key is pressed
    {
        if (flag == 0) 
        {
            flag = 1; // Move the selection to the second option
        }
        else 
        {
            if (count == 0 || count == 1)
                count++; // Increment the menu option index
        }
    }
    else if (key == 11 && menu_flag == 1) // If the "up" key is pressed
    {
        if (flag == 1) 
        {
            flag = 0; // Move the selection to the first option
        }
        else 
        {
            if (count == 1 || count == 2)
                count--; // Decrement the menu option index
        }
    }
    else if (key == 2 && menu_flag == 1) // If the "exit" key is pressed
    {
        menu_flag = 0; // Exit the menu
    }
    if (key == 1)  // Handle the "select" key press (key == 1)
    {
        if (flag == 1) 
        {
            count1 = count + 1; // Select the second option
        }
        else 
        {
            count1 = count; // Select the first option
        }
        if (count1 == 0)   // Update `menu_flag` based on the selected option
        {
            menu_flag = 2; // Navigate to menu screen 2
        }
        else if (count1 == 1) 
        {
            menu_flag = 3; // Navigate to menu screen 3
        }
        else if (count1 == 2) 
        {
            menu_flag = 4; // Navigate to menu screen 4
        }
        else if (count1 == 3) 
        {
            menu_flag = 5; // Navigate to menu screen 5
        }
    }
    if (menu_flag == 1)  // If still in the menu screen, update the display
    {
        display(); // Display the menu options
    }
    if (key == 2 && menu_flag == 1)  // Handle exit logic again for redundancy (key == 2)
    {
        menu_flag = 0; // Exit the menu
    }
}
void view_log(void)
{
    clcd_print("  TIME     EV SP ", LINE1(0));    // Display the header for the log view on the CLCD
    static unsigned int s_count;            // Index for navigating stored logs
    static unsigned int store_address = 0;  // EEPROM address for reading logs
    static unsigned int temp_store = 0;     // Temporary counter for logs read
    if (temp_store < event_count && temp_store < 10)   // Check if there are events to process and limit to the first 10 events
    {
        unsigned int i = 0; // Index for storing log data
        while (i < 16) 
        {
            if (i == 0) 
            {
                stored_event[temp_store][i] = temp_store % 10 + '0';
                i++;
            }
            if (i == 1 || i == 10 || i == 13) 
            {
                stored_event[temp_store][i] = ' ';
                i++;
            }
            else 
            {
                stored_event[temp_store][i] = read_EEPROM(store_address);
                i++;
                store_address++;
            }
        }
        stored_event[temp_store][i] = '\0'; // Null-terminate the log string
    }
    temp_store++;            // Move to the next event log
    if (event_index1 == 0)  // If no logs are available, display a message
    {
        clcd_print("NO LOGS         ", LINE1(0)); // Display no logs header
        clcd_print("TO DISPLAY :    ", LINE2(2)); // Display no logs message
    }
    else 
    {
        if (key == 11) // Up key pressed
        {
            if (s_count > 0) 
            {
                s_count--; // Move to the previous log
            }
        }
        else if (key == 12) // Down key pressed
        {
            if (s_count < 9) 
            {
                s_count++; // Move to the next log
            }
        }
        clcd_print(stored_event[s_count], LINE2(0));  // Display the current log on the CLCD
    }
    if (key == 2 && menu_flag == 2)   // Exit the log view and return to the menu if the exit key is pressed
    {
        menu_flag = 1; // Return to the menu
    }
}


void clear_log(void)  //function defination for clear log
{
    clcd_print("Clearing logs...  ", LINE1(0));
    clcd_print("Wait a minute    ", LINE2(0));    // Display messages on the CLCD to indicate log clearing
    event_index1 = 0;         // Reset the event index to indicate no logs are available
    if (delay++ == 3000)     // Implement a non-blocking delay mechanism
    {
        CLEAR_DISP_SCREEN;         // Clear the CLCD screen after the delay
        menu_flag = 1;   // Reset the menu flag to return to the menu screen
    }
}

void current_time(void)
{
    hour = (time[0] - '0') * 10 + (time[1] - '0');  // Extract and calculate the current hour from the `time` array
    min = (time[3] - '0') * 10 + (time[4] - '0');   // Extract and calculate the current minute from the `time` array
    sec = (time[6] - '0') * 10 + (time[7] - '0');   // Extract and calculate the current second from the `time` array
}

void update_time_in_ds1307(void)
{
    char hr_bcd = ((hour / 10) << 4) | (hour % 10);    // Convert hour to BCD format for DS1307
    char min_bcd = ((min / 10) << 4) | (min % 10);    // Convert hour to BCD format for DS1307
    char sec_bcd = ((sec / 10) << 4) | (sec % 10);   // Convert second to BCD format for DS1307
    write_ds1307(HOUR_ADDR, hr_bcd);  // Write the converted BCD values to the corresponding DS1307 registers
    write_ds1307(MIN_ADDR, min_bcd);
    write_ds1307(SEC_ADDR, sec_bcd);
}

void download_log(void)
{
    static unsigned int download_flag = 0;  // Static variable to keep track of the download process
    if (download_flag < 10)  // Check if there are logs to download (up to 10 logs)
    {
        if (event_index1 == 0)// If no logs are present
        {   
            if (download_flag == 0)   
            {
                clcd_print("NO LOGS         ", LINE1(0)); // Display message on CLCD indicating no logs to download
                clcd_print("TO DOWNLOAD :   ", LINE2(0));
                puts("NO LOGS TO DOWNLOAD :(  \n");  // Print message through UART
            }
        }
        else
        {
            static unsigned int store_address = 0;  // Static variables to track the EEPROM address and logs being processed
            static unsigned char temp_store = 0;
            clcd_print("Downloading..", LINE1(0));  // Display downloading status on the CLCD
            clcd_print("Through UART...", LINE2(0));
            for (int k = 1000; k--;);               // Small delay loop for visual effect
            if (temp_store < event_count && temp_store < 10)  // If logs are available and within the limit (10 logs)
            {
                unsigned char i = 0;
                while (i < 14)   // Read a log entry from EEPROM and send it through UART
                {
                    putch(read_EEPROM(store_address)); // Send a single character
                    store_address++;                  // Increment the EEPROM address
                    if(i == 7 || i == 10)
                    {
                        putch(' ');
                          i++;
                    }
                    i++;                              // Increment the character count
                }
                puts("\n\r"); // Add a new line and carriage return for UART display
            }
            else
            {
                store_address = 0;  // Reset the address and temporary log counter once logs are processed
                temp_store = 0;
            }
            temp_store++;   // Move to the next log
        }
        download_flag++;  // Increment the download flag to track progress
    }
    if (delay++ == 30000)   // Non-blocking delay to reset the screen and download flag
    {
        CLEAR_DISP_SCREEN; // Clear the CLCD screen
        menu_flag = 1;     // Return to the menu screen
        download_flag = 0; // Reset the download flag
    }
}


void display_time(void)
{
    // Display the current time on the LCD in HH:MM:SS format.   
    clcd_putch(time[0], LINE2(0)); // Display the tens digit of hours
    clcd_putch(time[1], LINE2(1)); // Display the ones digit of hours
    clcd_putch(':', LINE2(2)); // Display the separator (:) between hours and minutes
    clcd_putch(time[2], LINE2(3)); // Display the tens digit of minutes
    clcd_putch(time[3], LINE2(4)); // Display the ones digit of minutes
    clcd_putch(':', LINE2(5)); // Display the separator (:) between minutes and seconds
    clcd_putch(time[4], LINE2(6)); // Display the tens digit of seconds
    clcd_putch(time[5], LINE2(7)); // Display the ones digit of seconds
}

void set_time(void)
{
    static unsigned int SET_ONCE = 0; // Variable to ensure the function is executed only once for initialization
    if(SET_ONCE == 0) 
    {
        SET_ONCE = 2;                                    // Set flag to prevent re-entry into initialization block
        CLEAR_DISP_SCREEN;                              // Clear the display screen
        get_time();                                    // Retrieve the current time from the DS1307 RTC
        clcd_print("HH:MM:SS          ", LINE1(2));   // Display prompt for time setting
        clcd_print("                ", LINE2(0));    // Clear the second line for time display
        current_time();                             // Set the current time variables (hour, minute, second)
    } 
    static unsigned long wait = 0;           // Delay counter for setting time
    static int current_field = 0;           // Track which time field (hour, minute, second) is being edited
    if (wait <= 1000)
    {
        clcd_putch((hour / 10) + '0', LINE2(0));         // Display tens place of hour
        clcd_putch((hour % 10) + '0', LINE2(1));        // Display ones place of hour
        clcd_putch(':', LINE2(2));                     // Separator between hours and minutes
        clcd_putch((min / 10) + '0', LINE2(3));       // Display tens place of minute
        clcd_putch((min % 10) + '0', LINE2(4));      // Display ones place of minute
        clcd_putch(':', LINE2(5));                  // Separator between minutes and seconds
        clcd_putch((sec / 10) + '0', LINE2(6));    // Display tens place of second
        clcd_putch((sec % 10) + '0', LINE2(7));   // Display ones place of second
    }
    else if (wait > 1000 && wait <= 1800)   // Display blank spaces to indicate the currently editable field (hour, minute, or second)
    {
        if (current_field == 0) 
        {
             clcd_putch(' ', LINE2(0)); // Blank space for hours tens place
            clcd_putch(' ', LINE2(1)); // Blank space for hours ones place
        }
        else if (current_field == 1) 
        {
            clcd_putch(' ', LINE2(3)); // Blank space for minutes tens place
            clcd_putch(' ', LINE2(4)); // Blank space for minutes ones place
        }
        else if (current_field == 2) 
        {
            clcd_putch(' ', LINE2(6)); // Blank space for seconds tens place
            clcd_putch(' ', LINE2(7)); // Blank space for seconds ones place
        }
    }
    else
    {
        wait = 0; // Reset wait counter after time has been set
    }
    wait++; // Increment the wait counter
    if (key == MK_SW11 && menu_flag == 5) // If button MK_SW11 is pressed
    {
        if (current_field == 0) hour = (hour + 1) % 24; // Increment hour (wraps around 24)
        else if (current_field == 1) min = (min + 1) % 60; // Increment minute (wraps around 60)
        else if (current_field == 2) sec = (sec + 1) % 60; // Increment second (wraps around 60)
    }
    else if (key == MK_SW12 && menu_flag == 5) // If button MK_SW12 is pressed
    {
        current_field = (current_field + 1) % 3; // Switch between hour, minute, and second fields
    }
    else if (key == MK_SW1 && menu_flag == 5) // If button MK_SW1 is pressed
    {
        update_time_in_ds1307(); // Update the time in the DS1307 RTC
        CLEAR_DISP_SCREEN; // Clear the display
        SET_ONCE = 0; // Reset the SET_ONCE flag
        menu_flag = 1; // Go back to the main menu
    }
    else if (key == MK_SW2 && menu_flag == 5) // If button MK_SW2 is pressed
    {
        CLEAR_DISP_SCREEN; // Clear the display
        SET_ONCE = 0; // Reset the SET_ONCE flag
        menu_flag = 1; // Go back to the main menu
    }
}

void get_time(void)
{
    // Read the hour, minute, and second data from the DS1307 RTC (Real-Time Clock)
    clock_reg[0] = read_ds1307(HOUR_ADDR);  // Get the hour register value
    clock_reg[1] = read_ds1307(MIN_ADDR);   // Get the minute register value
    clock_reg[2] = read_ds1307(SEC_ADDR);   // Get the second register value

    // Check if the clock is in 12-hour mode (bit 6 of hour register is set)
    if (clock_reg[0] & 0x40)
    {
        // Extract the hour in 12-hour format (0-12) and update the time array
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x01); // Tens place of hour
        time[1] = '0' + (clock_reg[0] & 0x0F);        // Ones place of hour
    }
    else
    {
        // Extract the hour in 24-hour format (00-23) and update the time array
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x03); // Tens place of hour
        time[1] = '0' + (clock_reg[0] & 0x0F);        // Ones place of hour
    }
    // Extract the minute (0-59) and update the time array
    time[2] = ':';                                  // Separator for minutes
    time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);   // Tens place of minute
    time[4] = '0' + (clock_reg[1] & 0x0F);          // Ones place of minute
    // Extract the second (0-59) and update the time array
    time[5] = ':';                                  // Separator for seconds
    time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);   // Tens place of second
    time[7] = '0' + (clock_reg[2] & 0x0F);          // Ones place of second
    time[8] = ' ';                                  // Space for display formatting
    time[9] = '\0';                                 // Null terminator to end the string
}

