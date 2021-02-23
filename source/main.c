/*	Author: Nicole Navarro
 *  Partner(s) Name: 
 *	Lab Section: 21
 *	Assignment: Lab #10  Exercise #2
 *	Video: https://youtu.be/XTpwnW_bEME
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
unsigned char y = 0;
unsigned char keypad = 0;
unsigned char unlocked = 0;
unsigned char count;

unsigned char bell = 0;
unsigned char tuneperiod = 200;
unsigned char A7;


enum Keypad_States{input,release}Keypad_State;
int KeypadTick(int Keypad_State){
	switch(Keypad_State){

	case input:

		x = GetKeypadKey();
	        if(x == '\0'){ keypad = 0X1F;}
	 	if(x == '1'){ keypad = 0X01;}
		if(x == '2'){ keypad = 0X02;}
		if(x == '3'){ keypad = 0X03;} 
		if(x == '4'){ keypad = 0X04;}
		if(x == '5'){ keypad = 0X05;}
		if(x == '6'){ keypad = 0X06;} 
		if(x == '7'){ keypad = 0X07;} 
		if(x == '8'){ keypad = 0X08;} 
		if(x == '9'){ keypad = 0X09;}
		if(x == '*'){ keypad = 0X0E;} 
		if(x == '0'){ keypad = 0X00;} 
		if(x == '#'){ keypad = 0X0F;} 
			    
		
		if(keypad == 0x0F){
			count = 1;
		}
		else if((count == 1) && (keypad == 0x01)){
			count = 2;
		}
		else if((count == 2) && (keypad == 0x02)){
			count = 3;
		}
		else if((count == 3) && (keypad = 0x03)){
			count = 4;
		}
		else if((count == 4) && (keypad == 0x04)){
			count = 5;
		}
		else if((count == 5) && (keypad == 0x05)){
			count = 0;
			unlocked = 1; 	
		}
		else{
			count = 0;
		}
		Keypad_State = release;
		break;
	case release:
		if((GetKeypadKey() != '\0') && (y != 0)){
			y = 0;
			Keypad_State = input;
		}
		else if(GetKeypadKey() == '\0'){
			y = 1;
			Keypad_State = release;
		}
		else{
			Keypad_State = release;
		}
		break;
	default:
		Keypad_State = input;
		break;
	}
	return Keypad_State;

}

enum Lock_States{lock}Lock_State;
int LockTick(int Lock_State){
	switch(Lock_State){
		case lock: 
			if(~PINB & 0x80){
                		unlocked = 0;
        		}
			Lock_State = lock;
			break;
		default: Lock_State = lock; break;
	} 
	return Lock_State;
}

enum Bell_States{wait_press, melody}Bell_State;
int BellTick(int Bell_State){

	switch(Bell_State){
		case wait_press:
			if(A7){
				Bell_State = melody;
			}
			else{
				Bell_State = wait_press;
			}
			break; 
		case melody:
			bell = 0x02; 
			break;
		default: Bell_State = wait_press; break;
	}
	return Bell_State;
}


enum Combine_States{combine}Combine_State;
int CombineTick(int Combine_State){

	switch(Combine_State){
		case combine:
			PORTB = unlocked + bell;
			Combine_State = combine;
			break;
		default: Combine_State = combine; break;
	}
return Combine_State;
}


int main(void) {
DDRB = 0x7F; PORTB = 0X00;
DDRC = 0XF0; PORTC = 0X0F;
DDRA = 0X00; PINA = 0XFF;

static task task1, task2, task3, task4;
task *tasks[] = {&task1, &task2, &task3, &task4};
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
task2.TickFct = &LockTick;

//TASK3: Doorbell
task3.state = start;
task3.period = 200; //because button check 200ms so this is the base
task3.elapsedTime = task3.period;
task3.TickFct = &BellTick;

//TASK4: Combine
task4.state = start;
task4.period = 50;
task4.elapsedTime = task4.period;
task4.TickFct = &CombineTick;

	TimerSet(1);
	TimerOn();

	unsigned short i;
        while(1){
		A7 = ~PINA & 0x80;	
		task3.period = tuneperiod;

		for(i=0; i<numTasks; i++){ //Scheduler code
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;

}

