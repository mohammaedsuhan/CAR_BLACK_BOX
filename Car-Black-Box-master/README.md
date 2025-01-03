# Car Black Box

## Introduction
The concept of Black Box is mainly heard by us in case of Aeroplanes. Upon a catastrophe, the Black Box is used to analyze the root cause of the issue. However, the purpose of Black Box can go beyond catastrophe analysis. Also known as an Event Data Recorder (EDR) or Accident Data Recorder (ADR), the Black Box can be installed in some automobiles to record various events. These events which are electronically sensed can often pro-actively detect any issue in the vehicle (ex: Engine fault) or monitor the fleet (ex: Drive over speeding), thereby doing pro-active maintenance of the Automotive Vehicle. 

By considering today’s busy life, every one wants to reach the destination as soon as possible by ignoring the rules and regulations. By implementing a Black Box which would log critical activities on the car and take appropriate actions in case of rash driving. As mentioned above the root cause of the negligence in the driving would be meeting the daily schedule and go off duty, or to earn extra money by illegal trips etc. So by implementing the mentioned solution, it would be easy to keep track of how the vehicle is being used, handled and control the efficiency of the vehicle.

The proposed solution is to log all the critical events like the gear shifts with current speed, the engine temperature, fuel consumption per trip, trip distance etc. The system should allow a password based access to the transport managers to view or download the Log to PC if required.

## Requirements
Initially run the Makefile from CBB_EEPROM_Loader folder. It stores the initial values in the External EEPROM. After this, run the Makefile from CBB_Main folder. This starts executing the Project on the PIC18F4580 Micro-controller board. An on-board CLCD screen is used to create an interactive interface.

Initially a Dashboard screen is displayed. It shows Current time, Last occurred event and Vehicle speed. Vehicle speed can be varied using the on-board POT (potentiometer). Matrix SW1 is used to register the Gear UP event. Matrix SW2 is used to register the Gear DOWN event. Matrix SW3 is used to register the Collision event. These event types are stored onto External EEPROM at any time in any display mode. Note that the other events are not displayed on dashboard. They are visible only in View log & Download log operation.

Matrix SW8 when pressed once takes the user to Password screen. Now the User has to provide the Correct Password. Initial Password is "0000" (changeable). Same Matrix SW8 is used to write '0' while Matrix SW9 is used to write '1'. With every entry, User can just see a '*' on the CLCD screen. User has to provide the password within 5 sec (changeable). If Password is correct then, a Log Menu screen opens up. Here, user can Scroll UP and DOWN using the matrix SW11 & matrix SW10 key respectively. If Password does not match, 4 (changeable) more tries are given to User. If User fails to provide the Correct Password after 5th try then, the screen will be locked for 60 sec (changeable). After 60 sec, User can retry again further 5 times.

Long pressing of matrix SW10 performs the rsepective functionalities:
1. **View log**: User can see last 10 possible Events stored in the Data log from the External EEPROM.
2. **Clear log**: A message appears on the screen saying that the Log is cleared from the External EEPROM. Now, in the View log mode; rest of the Events will not be visible to User. Scrolling operation will be ineffective.
3. **Change Password**: User can set a new Password. In this, after entering the Password once, the user will be asked to re-enter the Password. Now the user has to give the same pattern. Otherwise, it repeats enter & re-enter cycles.
4. **Download log**: User has to open the Serial terminal like Minicom or PuTTY etc. on the PC and connect the board with it using UART communication. After Long pressing the Download log option, the Data log will be printed on the Serial monitor.

Long pressing of matrix SW11 will take the User back to the Log Menu if a particular Option is opened. Again Long pressing of matrix SW11 will take the User back to the Dashboard.

## Snapshots
![image](https://user-images.githubusercontent.com/108017134/208310112-ac80d210-32d3-4d91-93fd-ff5bbd3feda2.png)
![image](https://user-images.githubusercontent.com/108017134/208310139-2d58fe8f-fa34-48ee-86ca-525193224560.png)
![image](https://user-images.githubusercontent.com/108017134/208310143-7bfeede7-144e-47de-ad84-e6bad2f41f05.png)
![image](https://user-images.githubusercontent.com/108017134/208310148-6dec32e6-23fd-45e5-8b2b-7a15c07584c8.png)
