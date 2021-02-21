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
			    case '1': PORTB = 0X01; break;
		    	    case '2': PORT1B= 0X02; break;
			    case '3': PORTB = 0X03; break;
			    case '4': PORTB = 0X04; break;
			    case '5': PORTB = 0X05; break;
			    case '6': PORTB = 0X06; break;
			    case '7': PORTB = 0X07; break;
			    case '8': PORTB = 0X08; break;
			    case '9': PORTB = 0X09; break;
			    case 'A': PORTB = 0X0A; break;
			    case 'B': PORTB = 0X0B; break;
			    case 'C': PORTB = 0X0C; break;
			    case 'D': PORTB = 0X0D; break;
		            case '*': PORTB = 0X0E; break;      
			    case '0': PORTB = 0X00; break;
			    case '#': PORTB = 0X0F; break;
			    default: PORTB = 0X1B; break; //should never occur

	    }

    }
    return 1;
}
