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
unsigned char y = 0;
unsigned char count = 0;
unsigned char unlocked = 0;
unsigned char keypad;
unsigned char locked = 1;
unsigned char B7;

enum Keypad_States{keypadnum,release}Keypad_State;
int KeypadTick(int state){
	switch(Keypad_State){
	
	case keypadnum:
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
			    default: keypad = 0X1B; break; //should never occur
		}
		if(keypad == 0x0F){
			count = 1;
		}
		else{
			count = 0;
		}
		if((count == 1) && (keypad == 0x01)){
			count = 2;
		}
		else{
			count = 0;
		}
		if((count == 2) && (keypad == 0x02)){
			count = 3;
		}
		else{
			count = 0;
		}
		if((count == 3) && (keypad = 0x03)){
			count = 4;
		}
		else{
			count = 0;
		}
		if((count == 4) && (keypad == 0x04)){
			count = 5;
		}
		else{
			count = 0;
		}
		if((count == 5) && (keypad == 0x05)){
			unlocked = 1; 	
		}
		else{
			unlocked = 0;
		}
		if(keypad != 0x1f){
			Keypad_State = release;
		}
		else{
		Keypad_State = keypadnum;
		}
		break;
	case release:
		if((keypad != 0x1f) && (y != 0)){
			y = 0;
			Keypad_State = keypadnum;
		}
		else if(keypad == 0x1f){
			y = 1;
			Keypad_State = release;
		}
		else{
			Keypad_State = release;
		}
		break;
	default:
		Keypad_State = keypadnum;
		break;
	}
	return Keypad_State;

}

enum Lock_States{lock}Lock_State;
int LockTick(int Lock_State){
	switch(Lock_State){
		case lock:
			if(B7 == 1){
				locked = 1;
			}
			Lock_State = lock;
			break;
		default: Lock_State = lock; break;
	}
	return Lock_State;
}


enum Combine_States{combine}Combine_State;
int CombineTick(int Combine_State){
	unsigned char output = 0;
	unsigned char lock;
	unsigned char c;
	unsigned char press = 0;

	switch(Combine_State){
		case combine:
			c = GetKeypadKey();
			if(c != '\0'){
				press = 2;
			}
			else{
				press = 0;
			} 
		       if(unlocked){
		       		lock = 1;
				press = press + 4;
		 	}
		       else if(locked){
	 			lock = 0;
				press = press + 8;
			}
			output = (lock + press) | (count << 4); 
			break;
		default:Combine_State = combine; break;
			 
	}
//	PORTB = output;
	return Combine_State;
}


int main(void) {
    
	DDRB = 0X7f; PORTB = 0X80;
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

	//TASK2: Lock
	task2.state = start;
        task2.period = 50;
        task2.elapsedTime = task2.period;
        task2.TickFct = &LockTick;

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
		B7 = PORTB >> 7;
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
