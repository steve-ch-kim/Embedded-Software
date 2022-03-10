/*
 * ACR Project 2.c
 *
 * Created: 2/7/2022 4:34:08 PM
 * Author : KemperFi, Steve Kim
 * We are implementing time as MILITARY TIME
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "avr.h"
#include "lcd.h"

struct dt {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};

void init_dt(struct dt *dt)  {
	dt -> year = 2021;
	dt -> month = 2;
	dt -> day = 7;
	dt -> hour = 3;
	dt -> minute = 59;
	dt -> second = 0;
}

int is_pressed (int r, int c) {
	DDRC = 0;
	PORTC = 0;
	SET_BIT(DDRC, r);
	CLR_BIT(PORTC, r);
	CLR_BIT(DDRC, c + 4);
	SET_BIT(PORTC, c + 4);
	NOP();
	avr_wait(1);
	return GET_BIT(PINC, c + 4) ? 0 : 1;
}

int get_key() {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (is_pressed(i,j)) {
				return i*4+j+1;
			}
		}	
	}
	return 0;
}

void advance_dt (struct dt *dt) { //assuming its military time 
	++dt -> second;
	if (60 == dt -> second ) {
		dt -> second = 0;
		++dt -> minute;
		if (60 == dt -> minute) {
			dt->minute = 0;
			++dt -> hour;
			if (24 == dt -> hour) {
				dt->hour = 0;
				++dt -> day;
				if (30 < dt -> day) { //not factoring in different days in a month  
					dt->day = 1;
					++dt -> month;
					if (12 < dt -> month) { 
						dt->month = 1;
						++dt -> year;
// 						if (9999 == dt -> year) { EXCEPTION CASE
// 							dt->year = 0;
// 							++dt -> year;
// 						}
					}
				}
			}
		}
	}
}

void print_dt (const struct dt *dt) {
	char date[17];	
	char time[17];

	lcd_pos(0,0);
	sprintf(date,"%02d / %02d / % 04d", dt -> month, dt ->day, dt ->year);
	sprintf(time,"%02d:%02d:%02d", dt -> hour, dt -> minute, dt -> second);

	lcd_pos(0, 0);
	lcd_puts2(date);
	lcd_pos(1, 0);
	lcd_puts2(time);
}	

int is_clock_valid(const struct dt *dt)
{
	if (dt->month > 12)
	{
		return 0;
	}
	
	switch(dt->month)
	{
		case 1:
		if (dt->day > 31) return 0;
		case 2:
		if (dt->day > 28) return 0;
		case 3:
		if (dt->day > 31) return 0;
		case 4:
		if (dt->day > 30) return 0;
		case 5:
		if (dt->day > 31) return 0;
		case 6:
		if (dt->day > 30) return 0;
		case 7:
		if (dt->day > 31) return 0;
		case 8:
		if (dt->day > 31) return 0;
		case 9:
		if (dt->day > 30) return 0;
		case 10:
		if (dt->day > 31) return 0;
		case 11:
		if (dt->day > 30) return 0;
		case 12:
		if (dt->day > 31) return 0;
	}
	
	if (dt->year < 0)
	{
		return 0;
	}
	
	if (dt->hour > 24)
	{
		return 0;
	}
	
	return 1;
}

void update_time(struct dt *dt)
{
	if (dt->minute >= 60)
	{
		dt->minute -= 60;
		dt->hour++;
	}
	
	if (dt->second >= 60)
	{
		dt->second -= 60;
		dt->minute++;
	}
}

int main(void)
{
	struct dt dt1;
	lcd_init();
	init_dt(&dt1);
	print_dt(&dt1);
	
	char keys[17] = {' ','1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
	
	for (;;)
	{
		// create date array to store the 8 digits for the date
		char date[8];
		int date_counter = 7;
		
		// create int key variable to store the inputs
		// first input will always return 0 as nothing is being pressed
		int key = get_key();
		
		// keep looping until a key is pressed
		// the while loop will only exit when a key returns a non zero value, which means a key was pressed
		while (!key)
		{
			key = get_key();
		}
		
		// initialize the first digit in the array
		date[0] = keys[key];
		lcd_clr();
		lcd_put(date[0]);
		
		// loop for 7 more iterations to finish the date array
		while (date_counter)
		{
			// since a key was pressed prior to this, keep looping until get_key returns 0 
			// should be the first iteration as when it gets called no key is pressed 
			while(key) {
				key = get_key();
			}
			
			// once we know we're ready to press the next key, use the same logic to loop until a key is pressed
			while (!key) {
				key = get_key();
			}
			// initialize the rest of the date array
			date[8 - date_counter] = keys[key];
			lcd_put(date[8 - date_counter]);
			date_counter--;
		}
		
		// update the data for the dt struct (labeled as dt1 since there were errors with "struct dt dt" having the same name
		dt1.month = (date[0] - '0') * 10;
		dt1.month += (date[1] - '0');
		dt1.day = (date[2] - '0') * 10;
		dt1.day += (date[3] - '0');
		dt1.year = (date[4] - '0') * 1000;
		dt1.year += (date[5] - '0') * 100;
		dt1.year += (date[6] - '0') * 10;
		dt1.year += (date[7] - '0');
		
		// same logic with time, but position the lcd to the second row
		lcd_pos(1, 0);
		char time[6];
		int time_counter = 5;
		
		while (key)
		{
			key = get_key();
		}
		
		while(!key) {
			key = get_key();
		}
		
		time[0] = keys[key];
		lcd_put(time[0]);
		
		while (time_counter)
		{	
			while (key)
			{
				key = get_key();
			}
			
			while (!key) {
				key = get_key();
			}
			time[6 - time_counter] = keys[key];
			lcd_put(time[6 - time_counter]);
			time_counter--;
		}
		
		dt1.hour = (time[0] - '0') * 10;
		dt1.hour += (time[1] - '0');
		dt1.minute = (time[2] - '0') * 10;
		dt1.minute += (time[3] - '0');
		dt1.second = (time[4] - '0') * 10;
		dt1.second += (time[5] - '0');
		
		// update_time: updates the time when minutes or seconds are greater than 60
		// we do not need to update the date as the number of days or months do not carry over; it will be invalid if the months > 12, etc
		/*
			example: 12:10:65 -> 12:11:05, 24:60:57 -> 25:00:57
		*/
		update_time(&dt1);
		
		// if the time/date is invalid, print "INVALID" on the LCD screen rather than the formatted time/date (instructions)
		// run the next iteration of the loop and ignore the rest
		if (!is_clock_valid(&dt1))
		{
			lcd_pos(1, 0);
			lcd_puts2("INVALID");
			avr_wait(1000);
			continue;
		}
		
		// print the formatted output of the time/date onto the LCD
		print_dt(&dt1);
		
		// same logic as before: wait for get_key to return 0 so we know a button isn't being pressed
		while (key)
		{
			key = get_key();
		}
		
		// then loop again until get_key returns a nonzero value, so we know a button was pressed
		while(!key) {
			key = get_key();
		}
		
		// if that button equals '*', run the clock and continuously update the time/date 
		if (keys[key] == '*')
		{
			while (key)
			{
				key  = get_key();
			}
			
			// stop the clock when you press/hold down '*'
			// for some reason, holding down '*' registers more efficiently to stop the clock
			while (!key)
			{
 				print_dt(&dt1);
				avr_wait(1000);
 				advance_dt(&dt1);
				key = get_key();
			}
		}
		
		avr_wait(1000);
	}
	return 0;
}

