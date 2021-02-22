/*	Author: Nicole Navarro
 *  Partner(s) Name: 
 *	Lab Section: 21
 *	Assignment: Lab #10  Exercise #1
 *	Video: https://youtu.be/AbkH4SwXydM
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
#endif

unsigned char x;
unsigned char y;
unsigned char keypad;
unsigned char button;

enum Keypad_States{output_keypad}Keypad_State;
int KeypadTick(int state){
	switch(Keypad_State){
	
	case output_keypad:
		x = GetKeypadKey();
		switch(x){
			 case '\0': keypad = 0X1F; break;
			    case '1': keypad = 0X01; break;
		    	    case '2': keypad = 0X02; break;
			    case '3': keypad = 0X03; break;
			    case '4': keypad = 0X04; break;
			    case '5': keypad = 0X05; break;
			    case '6': keypad = 0X06; break;
			    case '7': keypad = 0X07; break;
			    case '8': keypad = 0X08; break;
			    case '9': keypad = 0X09; break;
			    case 'A': keypad = 0X0A; break;
			    case 'B': keypad = 0X0B; break;
			    case 'C': keypad = 0X0C; break;
			    case 'D': keypad = 0X0D; break;
		            case '*': keypad = 0X0E; break;
			    case '0': keypad = 0X00; break;
			    case '#': keypad = 0X0F; break;
			    default: PORTB = 0X1B; break; //should never occur
		}
		Keypad_State = output_keypad;
		break;
	default:
		Keypad_State = output_keypad;
		break;
	}
	return Keypad_State;

}

enum Button_States{buttonpress}Button_State;
int ButtonPressTick(int Button_State){
	switch(Button_State){
		case buttonpress: 
			y = GetKeypadKey();
			if(x == '\0'){
				button = 0;
			}
			else{ button = 1;}
			break;
		default: Button_State = buttonpress; break;
	}
	return Button_State;
}


enum Combine_States{combine}Combine_State;
int CombineTick(int Combine_State){
	unsigned char output;

	switch(Combine_State){
		case combine: output = keypad | (button << 7); break;
		default: Combine_State = combine; break;
	}
	PORTB = output;
	return Combine_State;
}


int main(void) {
    
	unsigned char x;
	DDRB = 0XFF; PORTB = 0X00;
	DDRC = 0XF0; PORTC = 0X0F;

	static task task1, task2, task3;
	task *tasks[] = {&task1, &task2, &task3};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;
	
	// TASK1: Keypad buttons
	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &KeypadTick;

	//TASK2: Button press
	task2.state = start;
        task2.period = 50;
        task2.elapsedTime = task2.period;
        task2.TickFct = &ButtonPressTick;

	//TASK3: Combine
	task3.state = start;
        task3.period = 50;
        task3.elapsedTime = task3.period;
        task3.TickFct = &CombineTick;

	unsigned long GCD = tasks[0]->period;
	for(unsigned i=1; i<numTasks; i++) {
		GCD = findGCD(GCD,tasks[i]->period);
	}

	TimerSet(GCD);
	TimerOn();

	unsigned short i;
        while(1){	
		for(i=0; i<numTasks; i++){ //Scheduler code
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += GCD;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;

}
