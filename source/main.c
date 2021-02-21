/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "keypad.h"
#endif

int main(void) {
    
	unsigned char x;
	DDRB = 0XFF; PORTB = 0X00;
	DDRC = 0XF0; PORTC = 0X0F;

    
    while (1) {
	    x = GetKeypadKey();
	    switch(x){
		    case '\0': PORTB = 0X1F; break;

	    }

    }
    return 1;
}
