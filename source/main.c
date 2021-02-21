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

	static _task task1, task2, task3;
	_task *tasks[] = {&task1, &task2, &task3};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;
	
	// TASK1: Keypad buttons
	task1.state = start;
	task1.period = 50;
	task1.elapsedTIme = task1.period;
	task1.TIckFct = &KeypadTick;

	//TASK2: Button press
	task2.state = start;
        task2.period = 50;
        task2.elapsedTIme = task2.period;
        task2.TIckFct = &ButtonPressTick;

	//TASK3: Combine
	task3.state = start;
        task3.period = 50;
        task3.elapsedTIme = task3.period;
        task3.TIckFct = &CombineTick;

	unsigned long GCD = tasks[0]->period;
	for(unsigned i=1; i<numTasks; i++) {
		GCD = findGCD(GCD,tasks[i]->period);
	}

	TimerSet(GCD);
	TimerOn();

	unsigned short i;
        while(1){	
		for(i=0; i<numTasks; i++){ //Scheduler code
			if(tasks[i]->elapsedTIme == tasks[i]->period){
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
