/*
 * ACR Project 4.c
 *
 * Created: 3/1/2022 4:34:08 PM
 * Author : KemperFi, Steve Kim
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include "avr.h"
#include "lcd.h"

#define C 523.3
#define C_HIGH 1046.5
#define G 784

struct note {
	int freq;
	int duration;
};

struct note SONG[] = {
	{C, 1}, // UNLOCK
	{G, 1}, // LOCK
	{C_HIGH, 1}, // FAILED ATTEMPT
};

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

void Playnote(const struct note n){
	int i, k, TH, TL;
	k = n.duration * n.freq;
	TH = ((100000) / n.freq) / 2;
	TL = TH;
	
	for(i = 0; i < k; ++i){
		SET_BIT(PORTB,3);
		avr_wait2(TH);
		CLR_BIT(PORTB,3);
		avr_wait2(TL);
	}
}

void avr_wait2(unsigned short msec)
{
	TCCR0 = 2;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 8) * 0.00001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

void enter_pwd(char keys[17], int key, char *password) {
	while(!key) {
		key = get_key();
	}
	
	password[0] = keys[key];
	lcd_pos(1, 0);
	lcd_put(password[0]);
	
	int counter = 3;
	
	while (counter > 0) {
		while (key) {
			key = get_key();
		}
		
		while (!key) {
			key = get_key();
		}
		
		password[4 - counter] = keys[key];
		lcd_put(password[4 - counter]);
		counter--;
	}
	
	password[4] = '\0';
}

int main(void)
{
	lcd_init();
	lcd_clr();
	SET_BIT(DDRB, 3);
	
	// store all keys in key array 
	char keys[17] = {' ', '1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};
		
	// store all passwords in passwords array (10 is a testing number)
	char all_passwords[10][5];
	
	// for incrementing and adding to all_passwords
	int password_counter = 0;
	
	lcd_pos(0, 0);
	lcd_puts2("MAKE PASSWORD:");
	
	int key = get_key();
	// make the first password and store it as the current password
	char password[5];
	enter_pwd(keys, key, password);
	
	// add to all_passwords
	strcpy(all_passwords[password_counter], password);

	for(;;) {
		avr_wait(200);
		lcd_clr(); 
		lcd_pos(0, 0);
		lcd_puts2("ENTER PASSWORD:");
		Playnote(SONG[1]);
		
		while (key) {
			key = get_key();
		}
		
		// get password as first try 
		char pwd[5];
		enter_pwd(keys, key, pwd);
		avr_wait(200);
		
		// if the current password and first try password aren't equal, get another password
		while (strcmp(password, pwd) != 0) {
			lcd_clr();
			lcd_pos(0, 0);
			lcd_puts2("WRONG PASSWORD!");
			Playnote(SONG[0]);
			avr_wait(1000);
			
			lcd_clr();
			lcd_pos(0, 0);
			lcd_puts2("ENTER PASSWORD:");
			
			while (key) {
				key = get_key();
			} 
			
			enter_pwd(keys, key, pwd);
			avr_wait(200);
		}
		
		// if they are equal, switch to UNLOCK mode
		lcd_clr();
		lcd_pos(0, 0);
		lcd_puts2("UNLOCKED! PRESS");
		lcd_pos(1, 0);
		lcd_puts2("* TO CHANGE PWD!");
		Playnote(SONG[2]);
		
		key = get_key();
		
		while(!key) {
			key = get_key();
		}
		
		// if * is pressed, 
		if (keys[key] == '*') {
			lcd_clr();
			lcd_pos(0, 0);
			lcd_puts2("ENTER NEW PWD:");
			
			while (key) {
				key = get_key();
			}
			
			// enter new password
			enter_pwd(keys, key, pwd);
			avr_wait(200);
			
			// if new password is one that was used previously, loop until its a new password
			for (int i = 0; i < 10; i++) {
				if (all_passwords[i]) {
					if (strcmp(all_passwords[i], pwd) == 0) {
						lcd_clr();   
						lcd_pos(0, 0);
						lcd_puts2("OLD PASSWORD!");
						Playnote(SONG[0]);
						avr_wait(1000);
						
						lcd_clr();
						lcd_pos(0, 0);
						lcd_puts2("ENTER NEW PWD:");
						
						while (key) {
							key = get_key();
						}
						
						// enter new password
						enter_pwd(keys, key, pwd);
						i = 0;
						avr_wait(200);
					}	
				}
			}

			// update the current password as new password and increment password_counter
			password_counter++;
			strncpy(password, pwd, sizeof(password));
			// add new password to all_passwords array
			strcpy(all_passwords[password_counter], password);
			
			lcd_clr();
			lcd_pos(0, 0);
			lcd_puts2("UPDATED");
			lcd_pos(1, 0);
			lcd_puts2("PASSWORD!");
			avr_wait(1000);
		}
		// loop again to beginning of for loop with the new updated current_password
		
	}
	return 0;
}