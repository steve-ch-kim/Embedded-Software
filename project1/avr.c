#include <avr/io.h>
#include "avr.h"



void avr_wait(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

int main (void)
{
    SET_BIT(DDRB, 0); //DDRRB or PORT?
    CLR_BIT(DDRB, 1);
    while (1) {
	    if (GET_BIT(PINB, PB1) == 0) {
		    //PORTB ^= (1 << PB2);
		    SET_BIT(PORTB, 0); // calls set for portB f
		    avr_wait(500); //500ms delay
		    CLR_BIT(PORTB, 0);
		    avr_wait(500);
	    }
}}