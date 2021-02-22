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
unsigned char keypad;
unsigned char unlocked;
unsigned char locked = 1;
unsigned char A7;

enum Keypad_States{input,release}Keypad_State;
int KeypadTick(int Keypad_State){
	unsigned char count;
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
			unlocked = 0;
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
			if((PINB >> 7) == 1){
				locked = 1;
			}
			Lock_State = lock;
			break;
		default: Lock_State = lock; break;
	} 
	return Lock_State;
}

enum Bell_States{press, melody}Bell_State;
int BellTick(int Bell_State){
	unsigned char count;

	switch(Bell_State){
		case press:
			PWM_off();
			count = 0;
			if(A7){
				Bell_State = melody;
			}
			else{
				Bell_State = press;
			}
			break;
		case melody:
			PWM_on();
			if((count == 0) || (count == 2)){
				++count;
				set_PWM(392.00);
			}
			if((count ==1) || (count == 3)){
				++count;
				set_PWM(440.00);
			}
			if((count == 4) || (count == 5)){
				++count;
				set_PWM(523.25);
			}
			if(!A7 && (count > 5)){
				Bell_State = press;
			}
			else{
			Bell_State = melody;
			}
			break;
		default: Bell_State = press; break;
	}
	return Bell_State;
}


enum Combine_States{combine}Combine_State;
int CombineTick(int Combine_State){
	unsigned char output;

	switch(Combine_State){
		case combine:
			if(unlocked){
				output = 1;
			}
			else if(locked){
				output = 0;
			}
			break;
		default: Combine_State = combine; break;
	}
	PORTB = output;
	return Combine_State;
}


int main(void) {
    	DDRB = 0x7F; PORTB = 0X80;
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
	task3.period = 500;
	task3.elapsedTime = task3.period;
	task3.TickFct = &BellTick;

	//TASK4: Combine
	task4.state = start;
        task4.period = 50;
        task4.elapsedTime = task3.period;
        task4.TickFct = &CombineTick;

	unsigned long GCD = tasks[0]->period;
	for(unsigned i=1; i<numTasks; i++) {
		GCD = findGCD(GCD,tasks[i]->period);
	}

	TimerSet(GCD);
	TimerOn();

	unsigned short i;
        while(1){	
		A7 = ~PINA >> 7;
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

void set_PWM(double frequency){
	static double current_frequency;

	if(frequency != current_frequency){
		if(!frequency) {TCCR3B &= 0X08;}
		else { TCCR3B |= 0x03; }

		if(frequency < 0.954) { OCR3A = 0XFFFF; }

		else if(frequency > 31250) { OCR3A = 0X0000; }

		else { OCR3A = (short) (8000000 / (128*frequency)) - 1; }

		TCNT3 = 0;
		current_frequency = frequency;

	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);

	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

	set_PWM(0);
}

void PWM_off(){
	TCCR3A = 0X00;
	TCCR3B = 0X00;
}
