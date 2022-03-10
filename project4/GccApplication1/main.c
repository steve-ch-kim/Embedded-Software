/*
 * ACR Project 4.c
 *
 * Created: 3/1/2022 4:34:08 PM
 * Author : KemperFi, Steve Kim
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "avr.h"
#include "lcd.h"

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

int get_sample() {
	SET_BIT(ADMUX, 6);
    ADCSRA = 0b11000000;
	while (GET_BIT(ADCSRA, 6)) {}
	return ADC;
}

int main(void)
{
	lcd_init();
	lcd_clr();

	for(;;) {
		float count = 0.0;
		float sum = 0.0;
		float average = 0.0;
		float max = 0.0;
		float min = 0.0;
		
		char line1 [50];
		char line2 [50];
		
		int sample = get_sample();
		sprintf(line1, "V %4.2f A %4.2f", (sample/1023.0)*5, (sample/1023.0)*5);
		sprintf(line2, "Mn %4.2f Mx %4.2f", (sample/1023.0)*5, (sample/1023.0)*5);
		lcd_pos(0, 0);
		lcd_puts2(line1);
		lcd_pos(1, 0); 
		lcd_puts2(line2);
		
		int key = get_key();
		
		while (!key)
		{
			key = get_key();
		}
		
		if (key == 13)
		{
			char line_1 [50];
			char line_2 [50];
			
			while (key)
			{
				key = get_key();
			}
			
			while (!key)
			{	
				sample = get_sample();
				float s = (sample/1023.0)*5;
				count += 1;
				
				if (count == 1) {
					min = s;
					max = s;
					average = s;
					
					sprintf(line_1, "V %4.2f A %4.2f", s, average);
					sprintf(line_2, "Mn %4.2f Mx %4.2f", min, max);
					lcd_pos(0, 0);
					lcd_puts2(line_1);
					lcd_pos(1, 0);
					lcd_puts2(line_2);		
					avr_wait(500);
					key = get_key();
				}
				else {
					if (s < min) {
						min = s;
					}
					
					if (s > max) {
						max = s;
					}
					
					sum += s;
					average = sum/count;
					
					sprintf(line_1, "V %4.2f A %4.2f", s, average);
					sprintf(line_2, "Mn %4.2f Mx %4.2f", min, max);
					lcd_pos(0, 0);
					lcd_puts2(line_1);
					lcd_pos(1, 0);
					lcd_puts2(line_2);
					avr_wait(500);
					key = get_key();
				}
			}
			sprintf(line_1, "V %4.2f A ----", (sample/1023.0)*5);
			sprintf(line_2, "Mn ---- Mx ----", min, max);
			lcd_pos(0, 0);
			lcd_puts2(line_1);
			lcd_pos(1, 0);
			lcd_puts2(line_2);
			avr_wait(500);
 		}
	}
	return 0;
}

	 	
// 	for (;;)
// 	{
// 		// create date array to store the 8 digits for the date
// 		char date[8];
// 		int date_counter = 7;
// 		
// 		// create int key variable to store the inputs
// 		// first input will always return 0 as nothing is being pressed
// 		int key = get_key();
// 		
// 		// keep looping until a key is pressed
// 		// the while loop will only exit when a key returns a non zero value, which means a key was pressed
// 		while (!key)
// 		{
// 			key = get_key();
// 		}
// 		
// 		// initialize the first digit in the array
// 		date[0] = keys[key];
// 		lcd_clr();
// 		lcd_put(date[0]);
// 		
// 		// loop for 7 more iterations to finish the date array
// 		while (date_counter)
// 		{
// 			// since a key was pressed prior to this, keep looping until get_key returns 0 
// 			// should be the first iteration as when it gets called no key is pressed 
// 			while(key) {
// 				key = get_key();
// 			}
// 			
// 			// once we know we're ready to press the next key, use the same logic to loop until a key is pressed
// 			while (!key) {
// 				key = get_key();
// 			}
// 			// initialize the rest of the date array
// 			date[8 - date_counter] = keys[key];
// 			lcd_put(date[8 - date_counter]);
// 			date_counter--;
// 		}
// 		
// 		// update the data for the dt struct (labeled as dt1 since there were errors with "struct dt dt" having the same name
// 		dt1.month = (date[0] - '0') * 10;
// 		dt1.month += (date[1] - '0');
// 		dt1.day = (date[2] - '0') * 10;
// 		dt1.day += (date[3] - '0');
// 		dt1.year = (date[4] - '0') * 1000;
// 		dt1.year += (date[5] - '0') * 100;
// 		dt1.year += (date[6] - '0') * 10;
// 		dt1.year += (date[7] - '0');
// 		
// 		// same logic with time, but position the lcd to the second row
// 		lcd_pos(1, 0);
// 		char time[6];
// 		int time_counter = 5;
// 		
// 		while (key)
// 		{
// 			key = get_key();
// 		}
// 		
// 		while(!key) {
// 			key = get_key();
// 		}
// 		
// 		time[0] = keys[key];
// 		lcd_put(time[0]);
// 		
// 		while (time_counter)
// 		{	
// 			while (key)
// 			{
// 				key = get_key();
// 			}
// 			
// 			while (!key) {
// 				key = get_key();
// 			}
// 			time[6 - time_counter] = keys[key];
// 			lcd_put(time[6 - time_counter]);
// 			time_counter--;
// 		}
// 		
// 		dt1.hour = (time[0] - '0') * 10;
// 		dt1.hour += (time[1] - '0');
// 		dt1.minute = (time[2] - '0') * 10;
// 		dt1.minute += (time[3] - '0');
// 		dt1.second = (time[4] - '0') * 10;
// 		dt1.second += (time[5] - '0');
// 		
// 		// update_time: updates the time when minutes or seconds are greater than 60
// 		// we do not need to update the date as the number of days or months do not carry over; it will be invalid if the months > 12, etc
// 		/*
// 			example: 12:10:65 -> 12:11:05, 24:60:57 -> 25:00:57
// 		*/
// 		update_time(&dt1);
// 		
// 		// if the time/date is invalid, print "INVALID" on the LCD screen rather than the formatted time/date (instructions)
// 		// run the next iteration of the loop and ignore the rest
// 		if (!is_clock_valid(&dt1))
// 		{
// 			lcd_pos(1, 0);
// 			lcd_puts2("INVALID");
// 			avr_wait(1000);
// 			continue;
// 		}
// 		
// 		// print the formatted output of the time/date onto the LCD
// 		print_dt(&dt1);
// 		
// 		// same logic as before: wait for get_key to return 0 so we know a button isn't being pressed
// 		while (key)
// 		{
// 			key = get_key();
// 		}
// 		
// 		// then loop again until get_key returns a nonzero value, so we know a button was pressed
// 		while(!key) {
// 			key = get_key();
// 		}
// 		
// 		// if that button equals '*', run the clock and continuously update the time/date 
// 		if (keys[key] == '*')
// 		{
// 			while (key)
// 			{
// 				key  = get_key();
// 			}
// 			
// 			// stop the clock when you press/hold down '*'
// 			// for some reason, holding down '*' registers more efficiently to stop the clock
// 			while (!key)
// 			{
//  				print_dt(&dt1);
// 				avr_wait(1000);
//  				advance_dt(&dt1);
// 				key = get_key();
// 			}
// 		}
// 		
// 		avr_wait(1000);
// 	}

