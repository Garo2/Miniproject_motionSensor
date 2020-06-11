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

//variables for extra functions (editabele text)
char editable_text_busy[16];
char editable_text_empty[16];
int secondary_list_busy[16];
int secondary_list_empty[16];
int char_value = 64; // char (A)
int display_length = 0;
int character_number_busy = 0;
int character_number_empty = 0;
int check=0;
int init=1;

//variables for extra functions(cleaning times)
char cleaningTimes[20][12];
int buttonReleased = 0;
int timesCleaned = 0;
int displayCleanedIndex = 0;

int displayCleanedIndexReleased = 0;

/* Interrupt Service Routine */
void user_isr( void ) // calls repeatdly in a specific time
{
	if (IFS(0) & 0x100)		//will enter the if-statment every 100ms 
	{
		counter_for_timeout++;
		allowCheckPage100ms(); // to call the string every 100 ms
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
void allowCheckPage100ms()// 
{
	int get_switches = getsw();	// to call the function only in the first page
	if (get_switches & 0x2){
		
	}
	else if(get_switches & 0x1) {
		checkPage();
	}
	else {
		checkPage100ms();
	}
}

void setupText()
{
	editable_text_empty[0]='E';
	editable_text_empty[1]='M';
	editable_text_empty[2]='P';
	editable_text_empty[3]='T';
	editable_text_empty[4]='Y';
	editable_text_empty[5]=0x00;
	
	editable_text_busy[0]='B';
	editable_text_busy[1]='U';
	editable_text_busy[2]='S';
	editable_text_busy[3]='Y';
	editable_text_busy[4]=0x00;
	
}

void checkPage100ms(){ // call the changeText function every 100 ms.
	if(init == 1)
	{
		setupText();
		init=0;
	}
	
	if (PORTE == 0){
		//changeText(editable_text_empty, secondary_list_empty, character_number_empty); // calling function with passing parameters (empty)
		changeText(editable_text_empty, secondary_list_empty, character_number_empty);
		display_string(1, editable_text_empty);
		//display_string( 1,editable_text_empty);
		display_update();
		if (!check) // to let the character_number start from the begining once it changed the status and for switching for the second one
		{
			check=1;
			display_length=0; 
		}
	}
	
	 if (PORTE == 1){ // calling function for busy situation when PORTE =1 (busy) 
		changeText(editable_text_busy, secondary_list_busy, character_number_busy);
		display_string( 1,editable_text_busy);
		display_update();
		if (!check) // to let the character_number start from the begining once it changed the status and for switching for the second one
		{
			check=1;
			display_length=0;
		}
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
	//int butns = getBtns();			//get buttons values
	int get_switches = getsw();		//get switch value
		if (get_switches & 0x2) // switch 2
		{
			switch2Page2();		//call switch2Page2
		}
		else if (get_switches & 0x1)
		{
			switch2Page3();
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
	checkPage100ms();
	display_string(3,time_string);
	display_string( 2,"" );
	display_update();
	motion_sens = PORTB & 0x2;	//
	toStringDisplay(time_string, time, ':');
	cleaningTimesFun();
	
}

int switch2Page2()// what to do in page 2
{
	display_string( 0,"" );		//line 1
	display_string(1,"" );		//line 2
	display_string( 2,"" );		//line 3
	display_string( 3,"" );		//line 4
	display_update();			//update the screen every time we update the information
	display_string( 0,"PAGE 2" );
	display_string( 1, "AVERAGE USAGE" );
	display_string(2, "<2 <4 <6 6<");
	display_string(3, used_time_string);	//disable the information from used_time_string
	display_update();
	toStringDisplay(used_time_string, range_used_time, ' ');	//
}

void changeDisplayIndex()
{
	int butns = getbtns();
	
	
    if ((butns & 0x2) && (displayCleanedIndexReleased == 1))
    {
        displayCleanedIndexReleased = 0;
		displayCleanedIndex++;
		
    }
	
	 if ((butns & 0x1) && (displayCleanedIndexReleased == 1))
    {
	
        displayCleanedIndexReleased = 0;
		displayCleanedIndex--;
		if(displayCleanedIndex <=0) displayCleanedIndex = 0;
		
    }
    if ((butns & 0xf) == 0)
    {
        displayCleanedIndexReleased = 1;
    }
}



int switch2Page3()// what to do in page 2
{
	display_string( 0,"" );
	display_string( 1,"" );
	display_string( 2,"" );
	display_string( 3,"" );
	display_update();
	//motionSensorOn();	//get the information from motionSensorOn
	
	//checkPage100ms();
	//display_string(3,time_string);
	//display_string( 2,cleaningTimes[i][4]);
	display_string( 0,"PAGE 3" );
	cleaningTimesFun();
	changeDisplayIndex();
		display_string(1,cleaningTimes[displayCleanedIndex]);
	
display_string(2,cleaningTimes[displayCleanedIndex+1]);
	
	

display_string(3,cleaningTimes[displayCleanedIndex+2]);
	


	
	display_update();
}


char changeText(char* edit_character, int* temp_list, int character_number) // what to do in textChanging page instead of busy and available
{
	int butns = getbtns();	
	int get_switches = getsw();	
	if (get_switches & 0x8) // key for start editing the shown text on display
	{
		if (butns & 0x4) // increase the value of the character in ascii table
		{
			char_value++;
			temp_list[display_length] = char_value;
			changableString (edit_character, &char_value, temp_list); // call the fun and change in variables (char_value) value.
			//display_string( 1,edit_character );
			//display_update();
		}
		else if (butns & 0x2) // decrease the value of the char in ascii table
		{
			char_value--;
			temp_list[display_length] = char_value;
			changableString (edit_character, &char_value, temp_list);
			//display_string( 1,edit_character );
			//display_update();
		}
		else if (butns & 0x1) // jump to the next char in length
		/* increase the index of character and increase charcter_number variable value to declare it as the number of characters in line 
		use secondary list to save the recent value of character to reuse in when jumping to the next or previous character index in string*/
		{
			if (display_length >= 15){
			display_length = 0;
			}
			else if (display_length<=14){
				display_length++;
				character_number++; // while moving to next character // save the length of text to jump to it while going to the left side when display_length is =0;
			}
			char_value = temp_list[display_length]; // save value of character values into a temporary list to save them and not reuse them in second character array.
			changableString (edit_character, &char_value, temp_list); 
			//display_string( 1,edit_character );
			//display_update();
		}
		else if (butns & 0x8) // jump to the first previous char in length
		/* decreasing the display_length decide which index will be changed and presented */
		{
			if (display_length>0){
				display_length--;
			}
			else if (display_length <= 0){
				display_length = character_number;
			}
			char_value = temp_list[display_length];
			changableString (edit_character, &char_value, temp_list);
			//display_string( 1,edit_character );
			//display_update();
		}
	}
	else {display_length=0;} // to keep the default value of display_length;
}
void changableString (char* time_string_p, int* timep, int* temp_list) // convert time to string to show it on display
{
	if (*timep == 91 || *timep == 64){
		*timep = 32;
		time_string_p[display_length] = *timep;
		*timep = 91;
	}
	else if (*timep > 90)
	{
		*timep = 65;
		time_string_p[display_length] = *timep;
	}
	else if (*timep < 65)
	{
		*timep=90;
		time_string_p[display_length] = *timep;
	}
	else if (*timep<=90)
	{
		time_string_p[display_length] = *timep;
	}
	temp_list[display_length] = time_string_p[display_length] ;
	time_string_p[16] = 0;
}
int motionSensorOn() // when the sensor is on
{
	
	if (motion_sens && PORTE==0)	//when the sens is 1 and the port(led) is 0, then set the port(led) to 1 and display "Busy" on the screen.
	{
		PORTE = 1;
		check = 0;
		return 1;
	}
	else if (motion_sens && PORTE ==1)	//when the sens is 1 and the port(led) is 1, then set the port(led) to 0 and display "AVAILABLE" on the screen.
	{
		PORTE = 0;
		check =0;
		return 0;
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

//Karoline Extra Function (show last 20 times the bathroom has be cleaned)


void shiftCleaningTimes()
{
	int i=0;
	int j=0;
	for(i=0; i < 19; i++)
	{
		for(j=0; j < 11; j++)
		{
			cleaningTimes[i][j] = cleaningTimes[i+1][j];
		}
		cleaningTimes[i][11] = 0x00;
	}
	
}

void copyMemTime(int index)
{
	int i=0;
	for(i=0;i<11;i++)
	{
		cleaningTimes[index][i]=time_string[i];
	}
	cleaningTimes[index][11] = 0x00;
}

void addCleaningTime(int index)
{
	
	if(index >= 20)
	{
		shiftCleaningTimes();
		copyMemTime(19);
	}
	else
	{
		copyMemTime(index);
	}
	
}


void cleaningTimesFun()//(char* time_Stringpara)
{
    int butns = getbtns();
    if ((butns & 0x4) && (buttonReleased == 1))
    {
        buttonReleased = 0;
		addCleaningTime(timesCleaned);
        timesCleaned++;
    }
    else if ((butns & 0x4) == 0)
    {
        buttonReleased = 1;
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
void labwork( void ) // enable 
{
	return;
}