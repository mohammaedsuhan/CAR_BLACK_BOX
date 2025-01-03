/*
Name         : Prabhat Kiran
Date         : 30th November 2022
Description  : To implement the functionalities for the Car Black Box.
*/

#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "ds1307.h"
#include "i2c.h"
#include "matrix_keypad.h"
#include "adc.h"
#include "main.h"
#include "timer0.h"
#include "uart.h"
#include "ext_eeprom.h"

/* Global Variables Declaration */
unsigned char clock_reg [3];
unsigned char time [9];
unsigned char date [11];
unsigned short log_flag;
unsigned char password [5] ="0000";
static unsigned short key11_wait_flag;
static unsigned short menu_flag;
static unsigned short one_flag;

/* To Display the Time in CLCD */
void display_time (void)
{
	clcd_print (time, LINE2(0));
}

/* To Read the 24-Hr Format Time from the RTC */
static void get_time (void)
{
	clock_reg [0] = read_ds1307 (HOUR_ADDR);			//To store the Hours.
	clock_reg [1] = read_ds1307 (MIN_ADDR);				//To store the Minutes.
	clock_reg [2] = read_ds1307 (SEC_ADDR);				//To store the Seconds.

	/* To convert the Values read from RTC into Characters for displaying on the CLCD */
	if (clock_reg [0] & 0x40)
	{
		time [0] = '0' + ((clock_reg [0] >> 4) & 0x01);
		time [1] = '0' + (clock_reg [0] & 0x0F);
	}
	else
	{
		time [0] = '0' + ((clock_reg [0] >> 4) & 0x03);
		time [1] = '0' + (clock_reg [0] & 0x0F);
	}
	time [2] = ':';
	time [3] = '0' + ((clock_reg [1] >> 4) & 0x0F);
	time [4] = '0' + (clock_reg [1] & 0x0F);
	time [5] = ':';
	time [6] = '0' + ((clock_reg [2] >> 4) & 0x0F);
	time [7] = '0' + (clock_reg [2] & 0x0F);
	time [8] = '\0';

}

/* Basic Configuration */
void init_config(void)
{
	init_clcd ();				//CLCD Configuration.
	init_i2c ();				//I2C Configuration.
	init_ds1307 ();				//RTC Configuration.
	init_matrix_keypad ();		//Matrix Keypad Configuration.
	init_adc ();				//ADC Configuration.
	//TRISB0 = 0;
	//PORTB = 0;
	init_timer0 ();				//Timer Configuration.
	GIE = 1;					//Global Interrupt Enable.
	PEIE = 1;					//Peripheral Interrupt Enable.

	clcd_print ("TIME     EV  SP", LINE1(0));	//To display the Dashboard on the CLCD.

	/* To read the Password initially from the External EEPROM */
	int i;
	unsigned char add = 0xC8;
	for (i = 0; i < 4; i++)
	{
		password [i] = read_ext_eeprom (add++);
	}

}

int node_flag = 0;
unsigned char gear [8][3] = {"NU", "G1", "G2", "G3", "G4", "G5", "R ", "CR"};
/* Main Function */
void main (void)
{
	unsigned char key;
	init_config ();
	while(1)
	{
		key = read_switches (LEVEL_CHANGE);						//To read the Key Press.
		if (key == 8)											//If the SW8 is pressed, open the Password option.
		{
			node_flag = 1;
		}
		else if (key != 8 && node_flag == 0)					//If the SW8 is not pressed or if we want to go Back from Log Menu; Dashboard shall be displayed on the CLCD.
		{
			clcd_print ("TIME     EV  SP", LINE1(0));
			get_time ();										//To get the Time from RTC.
			display_time ();									//To display the Time on the CLCD.
			display_the_event (key);							//To display the Current Event on the CLCD.
		}
		if (node_flag == 1 && log_flag == 0)					//If SW8 is pressed, read the Password from the User.
		{
			give_password (key);								//To get the Password and check if it is Correct or not.
		}
		if (log_flag == 1)										//If the Password is matching, open the Log Menu.
		{
			key11_wait_flag = 0;
			display_menu (key);									//To display the Log Menu.
		}
	}
}

unsigned int adc_reg_val;
/* To Display the Event on the CLCD */
void display_the_event (unsigned char key)
{
	static unsigned int on_flag = 0;
	static unsigned int flag = 0;
	static unsigned int wait = 0;
	static unsigned int col_flag = 0;
	static int count = -1;

	if (on_flag == 0 && key == ALL_RELEASED)					//To display the default Event "ON" when the Car is powered ON.
	{
		clcd_print ("ON", LINE2(9));
	}
	else if (key == 1)											//To scroll Down the Gear events and store them along with Time and Speed in the External EEPROM.
	{
		flag = 1;
		on_flag = 1;
		store_event (count);
	}
	else if (key == 2)											//To scroll Up the Gear events and store them along with Time and Speed in the External EEPROM.
	{
		flag = 2;
		on_flag = 1;
		store_event (count);
	}
	else if (key == 3)											//If the Event is Collision, store "CR" along with Time and Speed in the External EEPROM.
	{
		flag = 3;
		on_flag = 1;
		store_event (7);
	}
	else if (key == ALL_RELEASED && flag == 1)					//If no SW is pressed and previous Key press was SW1, increment the 'count' to Scroll Down.
	{
		if (count < 6)
			count++;
		flag = 0;												//Reset the 'flag' to 0.
	}
	else if (key == ALL_RELEASED && flag == 2)					//If no SW is pressed and previous Key press was SW2, decrement the 'count' to Scroll Up.
	{
		if (count > 0)
			count--;
		flag = 0;												//Reset the 'flag' to 0.
	}
	else if (key == ALL_RELEASED && flag == 3)					//If no SW is pressed and previous Key press was SW3, display the Collision event on the CLCD.
	{
		clcd_print (gear [7], LINE2(9));
	}
	else if (count < 7 && count >= 0 && flag != 3)				//If 'count' lies between 0-6; display the Gear at which the Car is running on the CLCD.
	{
		clcd_print (gear[count], LINE2(9));
	}

	/* Read the Speed of the Car using ADC interfaced with the Potentiometer */
	adc_reg_val = (read_adc (CHANNEL4)) / 10.23;

	/* Display the Speed on the CLCD */
	clcd_putch ((adc_reg_val / 100) % 10  + '0', LINE2(12));
	clcd_putch ((adc_reg_val / 10) % 10  + '0', LINE2(13));
	clcd_putch (adc_reg_val % 10 + '0', LINE2(14));
}

unsigned short wait_flag;
extern unsigned short wait;
extern unsigned short flag_timer;
extern unsigned int blink;				//To enhance the Scope of Visibility of the variable from isr.c.
/* To Read the Password from the User and compare it with the Password stored in the External EEPROM */
void give_password (unsigned char key)
{
	static char attempt [4];
	static unsigned long delay;
	static unsigned int j;
	static unsigned int k;
	static unsigned int failure_flag;
	static unsigned int password_flag;
	static unsigned int timer_flag;
	static unsigned int no_attempts = 3;
	static unsigned int i;
	static unsigned int key_8_flag;
	static unsigned int enter_pwd;

	key = read_switches (STATE_CHANGE);							//To read the Password based on the Key Press.
	if (key == 8 && password_flag == 0)							//If SW8 is pressed, clear the CLCD for reading the Password from the User.
	{
		CLEAR_DISP_SCREEN;
		password_flag = 1;
		key_8_flag = 1;
	}

	if (password_flag == 1)										//To read the Password from the User.
	{
		if (enter_pwd == 0)
		{
			clcd_print (" Enter Password",LINE1(0));
		}
		if (i < 4)												//To make the Cursor Blink on the CLCD till the 4 Character Password is entered completely.
		{
			if (blink == 60)
				blink = 0;
			else if (blink % 2 == 0)							//Alternaltely display the '_'.
			{
				clcd_putch ('_', LINE2(i+6));
			}
			else if (blink % 2 == 1)							//Alternately display the ' '.
			{
				clcd_putch (" ", LINE2(i+6));
			}
		}
		if (j < 4)												//To check if the entered Password is of 4 Characters or not.
		{
			failure_flag = 0;
			if (key == 8 || key == 9 && key_8_flag == 0)		//To encrypt the Password entered as '*'.
			{
				if (key == 8)
					key = 0;
				if (key == 9)
					key = 1;
				if (key_8_flag == 0)							//To prevent the underlying code from being executed for the first time (i.e. when we enter the Password Menu).
				{
					attempt [j] = key + 48;
					clcd_putch ('*', LINE2(j++ + 6));
					i++;
				}
				key_8_flag = 0;
			}
		}
		if (j == 4)												//If the Password entered is of 4 Characters, check if the Password entered is Correct or not.
		{
			if (no_attempts > 1)								//To check if the Password Attempts are over or not.
			{
				no_attempts--;									//Decrement the Password Attempts before checking for the Current Password.
				for (k = 0; k < 4; k++)
				{
					if (attempt[k] != password[k])				//If the Passwords doesn't matches, display the Error message and Attempts left.
					{
						CLEAR_DISP_SCREEN;
						clcd_print ("Try Again",LINE1(0));
						clcd_putch (no_attempts + '0',LINE2(0));
						clcd_print ("Attempts", LINE2(2));
						for (delay = 50000; delay--;);			//To implement Blocking delay for displaying the messages properly.
						CLEAR_DISP_SCREEN;
						failure_flag = 1;
						i = 0;
						j = 0;
						break;
					}
				}
				if (failure_flag == 0)							//If the Password matches with the stored Password, display Successful message and Open the Log Menu.
				{
					no_attempts = 3;
					k = 0;
					i = 0;
					j = 0;
					CLEAR_DISP_SCREEN;
					clcd_print ("   SUCCESSFUL   ",LINE2(0));
					menu_flag = 2;
					for (delay = 50000; delay--;);
					CLEAR_DISP_SCREEN;
					password_flag = 0;
					log_flag = 1;
				}
			}
			else if (no_attempts <= 1)							//If the Password Attempts are Over, initiate a Delay of 60 seconds before the Next Attempt can be taken.
			{
				clcd_print ("Attempts Over   ",LINE1(0));
				clcd_print ("Please wait: ",LINE2(0));
				enter_pwd = 1;
				wait_flag = 1;
				if (wait > 0)									//To display the number of Seconds left before the Next Attempt.
				{
					clcd_putch ((wait /10) % 10 +'0', LINE2(13));
					clcd_putch (wait % 10 +'0', LINE2(14));
				}
				else											//After the Delay, repeat the process of Read Password from the User.
				{
					enter_pwd = 0;
					CLEAR_DISP_SCREEN;
					password_flag = 1;
					wait = 60;
					wait_flag = 0;
					i = 0;
					j = 0;
				}
			}
		}
	}
}

unsigned char data [11];
unsigned short count_event_flag;
unsigned int count_event;
/* To Store the Event on the External EEPROM */
void store_event (unsigned int count)
{
	int i = 0, j = 0;
	while (time [i] != '\0')									//To store the Time for the Event in 'data'.
	{
		if (time [i] != ':')									//Remove the ':' in Time array before storing the Values.
		{
			data [j++] = time [i];
		}
		i++;
	}

	data [j++] = (char) gear [count][0];						//To store the Event in 'data'.
	data [j++] = (char)gear[count][1];

	data [j++] = (adc_reg_val / 100) +'0';						//To store the Speed of the Car in 'data'.
	data [j++] = (adc_reg_val / 10) % 10 +'0';
	data [j++] = adc_reg_val % 10 +'0';

	/* To store the Data extracted above on the External EEPROM */
	for (j = 0; j < 11; j++)
	{
		write_ext_eeprom ((count_event * 10 + j), data[j]);
	}

	/* If the Count of the Events is greater than 10; Overwrite the Events from the Beginning whenever the New Event is stored */
	if (count_event < 10)
		count_event++;
	else
	{
		count_event_flag = 1;
		count_event = 0;
	}

}

char menu [5][17] = {"View log        ", "Download log    ","Clear log       ","Change PWD     ", "                "};
static unsigned short op_flag;
static unsigned short enter_flag;
static int key11_flag;
/* To Display the Log Menu on the CLCD */
void display_menu (unsigned char key)
{
	static unsigned short menu_wait;
	static unsigned short count;
	static unsigned short key11_wait;

	if (key11_wait_flag == 0)
	{
		clcd_putch ('*',LINE1(0));
		if (key == 10)													//If SW10 is pressed, check if it is a Short Press or Long Press.
		{
			menu_wait++;
			menu_flag = 1;
			clcd_putch (menu_wait % 10 + '0', LINE1(13));
			clcd_putch ((menu_wait / 10) % 10 + '0', LINE1(14));
			clcd_putch ((menu_wait /100) % 10 + '0', LINE1(15));
		}
		if (menu_wait >= 500 && key == 10)								//If the SW10 is pressed and 'menu_wait' value is greater than 500; it is a Long Press.
		{
			enter_flag = 1;												//Set the 'enter_flag' to Enter the Option of the Log Menu.
			CLEAR_DISP_SCREEN;
		}
		if (enter_flag == 1)											//If the 'enter_flag' is set 1; the Log Menu is displayed on the CLCD.
		{
			menu_wait = 0;
			menu_flag = 0;
			op_flag = 1;
			key11_flag = 0;
			if (count_event_flag == 1)									//If the Number of Events stored are more than 10; all the 10 Events currently available shall be processed.
			{
				log_fun (count, 10);
			}
			else														//If the Number of Events stored are less than 10; then the 'count_event' number of Events shall be processed.
				log_fun (count, count_event);
		}

		if (key == 11 && op_flag == 0 && one_flag == 0)					//If SW11 is pressed, check if it is a Short Press or Long Press.
		{
			key11_wait++;
			enter_flag = 0;
			menu_flag = 2;
			clcd_putch (key11_wait % 10 + '0', LINE1(13));
			clcd_putch ((key11_wait / 10) % 10 + '0', LINE1(14));
			clcd_putch ((key11_wait /100) % 10 + '0', LINE1(15));
		}
		if (key11_wait >= 1000 && key == 11)							//If the SW11 is pressed and 'menu_wait' value is greater than 1000; it is a Long Press.
		{
			key11_wait = 0;
			key11_wait_flag = 1;
		}
		if (key11_wait_flag == 1)										//If the SW11 is Long Press; the control shall go Back to the previous Menu or Dashboard.
		{
			menu_flag = 0;
			node_flag = 0;
			log_flag = 0;
			op_flag = 1;
			CLEAR_DISP_SCREEN;
		}
		if (key11_wait < 500)											//If the SW11 is Short Press; it shall Scroll Up on the Log Menu.
		{
			key11_wait_flag = 0;
			if (key == ALL_RELEASED && (menu_flag == 1 || menu_flag == 2))
			{
				one_flag = 0;
				key11_wait = 0;
				op_flag = 0;
				menu_wait = 0;
			}
			if (key == ALL_RELEASED && menu_flag == 1)
			{
				if (count < 3)											//The Scrolling Down is possible only 3 times to reach the end of the Log Menu.
					count++;
				menu_flag = 0;
			}
			if (key == ALL_RELEASED && menu_flag == 2)					//The Scrolling Up is possible only if 'count' is greater than 0.
			{
				if(count > 0)
					count--;
				menu_flag = 0;
			}
			if (count < 4 && count >= 0 && key == ALL_RELEASED && op_flag == 0)		//Display the Log Menu (2 options at a time) on the CLCD.
			{
				clcd_print (menu[count], LINE1(1));
				clcd_print (menu[count + 1], LINE2(1));
			}
		}
	}
}

/* To Process the Events based on the Option from the Log Menu */
void log_fun (unsigned short count, unsigned short limit)
{
	static signed short event_num;

	static unsigned short view_flag;
	static int i, j;
	static int wait;
	static unsigned char key;
	PEIE = 1;
	GIE = 1;
	clcd_print ("   Event Log   ", LINE1(0));

	key = read_switches (LEVEL_CHANGE);									//To read the Key press.
	if (key == 10)														//If SW10 is pressed, is shall Scroll Down on the entered Option from the Log Menu.
	{
		view_flag = 1;
	}
	if (key == 11)														//If SW11 is pressed, check if it is a Short Press or Long Press.
	{
		view_flag = 2;
		wait++;
	}
	if (wait > 500 && key == 11)										//If the SW11 is pressed and 'wait' becomes greater than 500; it is a Long Press.
	{
		wait = 0;
		key11_flag = 1;
	}
	if (key11_flag == 1)												//If the SW11 is Long Press, the control shall go back to the Log Menu.
	{
		one_flag = 1;
		menu_flag = 1;
		enter_flag = 0;
		op_flag = 0;
		view_flag = 0;
		log_flag = 1;
		CLEAR_DISP_SCREEN;
	}
	if (key == ALL_RELEASED && view_flag == 1)							//If the SW10 is pressed, the Scroll Down shall take place.
	{
		event_num++;													//Increment the 'event_num' to change the Menu.
		view_flag = 0;
	}
	if (key == ALL_RELEASED && view_flag == 2 && wait < 500)			//If the SW11 is Short Press, the Scroll Up shall take place.
	{
		wait_flag = 0;
		event_num--;													//Decrement the 'event_num' to change the Menu.
		view_flag = 0;
	}
	if (key11_flag == 0)
	{
		if (count == 0)													//To View the Events stored in the External EEPROM.
		{
			if (event_num < limit && event_num > 0)
			{
				clcd_putch (event_num +'0',LINE2(0));
				int j = 0;
				for (i = 1; i < 17; i++)
				{
					if (i == 3 || i == 6)
					{
						clcd_putch (':',LINE2(i));
					}
					else if (i == 9 || i == 12)
					{
						clcd_putch (' ',LINE2(i));
					}
					else
					{
						clcd_putch ((read_ext_eeprom(event_num* 10 + j)), LINE2(i));
						j++;
					}
				}
			}
		}
		else if (count == 1)											//To Download the Events stored in the External EEPROM.
		{
			int i, j = 0;
			PEIE = 0;
			GIE = 0;
			init_uart ();												//To Enable the UART for displaying the Events on the Laptop.
			for (int k = 0; k < count_event; k++)						//To display the Events on the Laptop.
			{
				j = 0;
				for (i = 0; i < 15; i++)
				{
					if (i == 2 || i == 5)
					{
						putch (':');
					}
					else if (i == 8 || i == 11)
					{
						putch (' ');
					}
					else
					{
						putch (read_ext_eeprom (k * 10 + j));			//Read the Event from External EEPROM.
						j++;
					}
				}
				puts ("\n\r");
			}
			CLEAR_DISP_SCREEN;
			clcd_print ("Files Downloaded", LINE2(0));
			for (long i = 500000; i--;);
			CLEAR_DISP_SCREEN;

			key11_flag = 1;
			enter_flag = 0;
			op_flag = 0;
			view_flag = 0;
			log_flag = 1;
			TXIE = 0;
			RCIE = 0;
		}
		else if (count == 2)											//To Clear the Events stored in the External EEPROM.
		{
			count_event = 0;
			clcd_print ("   Events erased", LINE2(0));
			for (long i = 500000; i--; );
			enter_flag = 0;
			op_flag = 0;
			view_flag = 0;
			log_flag = 1;
			key11_flag = 1;
		}
		else if (count == 3)											//To Change the Password for the Log Menu to be accessible.
		{
			static char mem_add = 0xC8;
			static int i, j;
			if (i < 4)
			{
				if (blink == 60)										//To blink the Cursor for reading the New Password.
					blink = 0;
				else if (blink % 2 == 0)
				{
					clcd_putch ('_', LINE2(i+6));
				}
				else if (blink % 2 == 1)
				{
					clcd_putch (' ', LINE2(i+6));
				}
				if (j < 4)												//To read the Password by the SW8 and SW9 Key press.
				{
					if (key == 8 || key == 9)
					{
						if (key == 8)
							key = 0;
						if (key == 9)
							key = 1;
						password [j] = key + 48;
						write_ext_eeprom (mem_add++, password[j]);		//To write the Password on the External EEPROM.
						clcd_putch ('*', LINE2(j++ + 6));
						i++;
					}
				}
			}
			if (i >= 4)													//If the Password is read successfully, display the Successful message.
			{
				j = 0;
				i = 0;
				CLEAR_DISP_SCREEN;
				clcd_print ("Password changed", LINE2(0));
				for (long i = 500000; i--; );
				CLEAR_DISP_SCREEN;

				enter_flag = 0;
				op_flag = 0;
				view_flag = 0;
				log_flag = 1;
				key11_flag = 1;
			}
		}
	}
}
