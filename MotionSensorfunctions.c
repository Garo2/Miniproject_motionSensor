/* MotionSensorfunctions.c
	This file written 2020 by Garo Malko and Karoline Malko

*/
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "Declatarions.h"  /* Declatations for these labs */

int motion_sens = 0;

int time[4] = {00,00,00,00};
int range_used_time[4] = {00,00,00,00};// first item describe the number of people has used the toilet less than 2 mins 
									   // second item describe the number of people has used the toilet 2 mins or less than 4. And so On 

char time_string[12];
char used_time_string[12];
int counter_for_timeout = 0;
int counter_for_sleep = 0;
int counter =0;
int port_turn_off_check =0;
int page_check=0;

/* Interrupt Service Routine */
void user_isr( void ) // calls repeatdly in a specific time
{
	if (IFS(0) & 0x100)		//will enter the if-statment every 100ms 
	{
		counter_for_timeout++;	
		
		if (counter_for_timeout>=10)	//
		{
			
			checkPage();
			timeCounter();
			counter_for_timeout =0;
			usedTime();
		}
		
		IFSCLR(0)=0x100;
	}
}
void usedTime() // the time the toilet has been used
{
    if(PORTE==1)		//when the portE(led) is turned on increase the counter by 1 and turn off checking on port 
    {
        counter++;
        port_turn_off_check=1;		
    }
    else if (port_turn_off_check == 1 && PORTE == 0)	//when the port_turn_off_check is turned on and PortE is off at the next round 
    {
        int temp = counter / 60;	//count how many mins has every person spend in the toilet 
        if (temp < 2){
            range_used_time[0]++;	//if the temp is less than 2 put the result in the first element in range_used_time array 
        }
        else if (temp >= 2 && temp < 4){
            range_used_time[1]++;	//if the temp is between 2 and 4 put the result in the sec element in range_used_time array
        }
        else if (temp >= 4 && temp < 6){
            range_used_time[2]++;	//if the temp is between 4 and 6 put the result in the third element in range_used_time array
        }
        else if (temp > 6){
            range_used_time[3]++;	//if the temp is bigger than 6 put the result in the fourth element in range_used_time array
        }
        port_turn_off_check = 0;	//set port_turn_off_check to 0 and counter to zero for the next round  
		counter=0;
     }
}
int checkPage () // check which slides and button are on and if there is none, choose default
{	
int butns = getBtns();			//get buttons values
int get_switches = getSw();		//get switch value
	if (get_switches & 0x2) // switch 2
	{
		switch2Page2();		//call switch2Page2
	}
	else if ((get_switches & 0x8) && (butns & 0x8)) // reset when button 4 is pushed and switch slide is on 
	{
		resetButton();		
	}
	else 
	{
		switch2Page1();
	}
}
void toStringDisplay (char* time_string_p, int* timep, char between) // convert time to string to show it on display
{
	int j=0;
	int i;
	for (i=0; i<4; i++)		
	{
		if(timep[i] < 10)
		{
			time_string_p[j++] = 48;
			time_string_p[j++] = 48 + timep[i];
			time_string_p[j++] = between;
		}
		else if (timep[i] > 9)
		{
			time_string_p[j++] = 48 + (timep[i]/10);
			time_string_p[j++] = 48 + (timep[i]%10);
			time_string_p[j++] = between;
		}
	}
	time_string_p[11] = 0;
}
int switch2Page1() // what to do in page 1
{
	display_string( 0,"" );
	display_string( 2,"" );
	display_string( 3,"" );
	display_update();
	motionSensorOn();	//get the information from motionSensorOn
	display_string( 0,"PAGE 1" );
	display_string(3,time_string);
	display_string( 2,"" );
	display_update();
	motion_sens = PORTB & 0x2;	//
	toStringDisplay(time_string, time, ':');
	
	
}

int switch2Page2()// what to do in page 2
{
	display_string( 0,"" );		//line 1
	display_string(1,"" );		//line 2
	display_string( 2,"" );		//line 3
	display_string( 3,"" );		//line 4
	display_update();			//update the screen every time we update the information
	display_string( 0,"PAGE 2" );
	display_string( 1, "USAGE AVERAGE" );
	display_string(2, "<2 <4 <6 6<");
	display_string(3, used_time_string);	//disable the information from used_time_string
	display_update();
	toStringDisplay(used_time_string, range_used_time, ' ');	//
}
int resetButton() // what to do in reset page 
{
	int i;
	for (i =0; i<4; i++)	//
	{
		time[i] =00;
		range_used_time[i]=00;	//set the whole array to 0
	}
	display_string( 0,"" );
	display_string(1,"" );
	display_string( 2,"" );
	display_string( 3,"" );		//empty the four lines 
	display_update();
	display_string(0,"RESET" );
	display_string(3,time_string);
	display_update();
	
}
int motionSensorOn() // when the sensor is on
{
	
	if (motion_sens && PORTE==0)	//when the sens is 1 and the port(led) is 0, then set the port(led) to 1 and display "Busy" on the screen.
	{
		PORTE = 1;
		display_string(1,"BUSY");
		display_update();
		return 1;
	}
	else if (motion_sens && PORTE ==1)	//when the sens is 1 and the port(led) is 1, then set the port(led) to 0 and display "AVAILABLE" on the screen.
	{
		PORTE = 0;
		display_string(1,"AVAILABLE");
		display_update();
		return 0;
	}
	else if (PORTE==1)		//display Busy when we turn on the ChipKit for first time when the port is on.
	{
		display_string(1,"BUSY");
		display_update();
	}
	else if (PORTE==0)		//display AVAILABLE when we turn on the shipKit for first time when the port is off.
	{
		display_string(1,"AVAILABLE");
		display_update();
	}
}
int timeCounter() // count actual time
{
	time[3]++;		//increasing the sec by one 
	if (time[3]>59)	//if sec is bigger than 59 set sec to 00 and increase the min by 1 
	{
		time[2]++;
		time[3] = 0;
	}
	if(time[2]>59)	//if min is bigger than 59 set min to 00 and increase the houre by 1 
	{
		time[1]++;
		time[2]=0;
		}
	if(time[1]>24)	//if houre is bigger than 59 set houre to 00 and increase the day by 1 
	{
		time[0]++;
		time[1]=0;
	}
}

void labinit( void ) // declare PORTS specifications
{
	TRISD |= 0xfe0;
	TRISE &= 0xffffff00;// set last 8 bits to 0
	TRISB |= 0x2; // pin index of pin 41 is RB1 = 0x2
	
	// Timer2 registers
	T2CON = 0;
	T2CONSET = 0x70; // set prescalling to 1:256 //in binary is 0111 , 1 bit to start/stops the timer
												 //• 1 bit for timer clock source select (TCS).
												 //Use 0 for internal peripheral clock
												 //• 1 bit for timer mode (connect 2 timers to form a 32-
												 //bit timer). Use 0 for 16-bit timer. 
	TMR2 =0; // set timer 2 to 0
	PR2 =31250; //((80000000/256)/10); 	
	IFSCLR(0) = 0x100; // clear the IFS
	T2CONSET = 0x8000; // start the timer // 	
	IECSET(0) = 0x100; // Enable timer 2 interrupts
	IPCSET(2) = 0xF; // Set priority for timer 2 interrupts
	enable_interrupt(); // Enable interrupts globally
}
int getSw() // get switches values
{
	int shiftPORTD = PORTD >> 8; // switches are connected to bits 8 - 11
	shiftPORTD = shiftPORTD & 0xf; // last digit (4 bits)
	return shiftPORTD;
}
int getBtns() // get buttons values
{
	int shiftPORTD = PORTD >> 4;// buttons are connected to bits 4,5,6 and 7
	shiftPORTD = shiftPORTD & 0xf;// comparing between 4 bits which has the sum 15
	return shiftPORTD;
}
void labwork( void ) // enable 
{
	return;
}