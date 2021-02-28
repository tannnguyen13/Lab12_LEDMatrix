/*	Author: Tann Nguyen
 *  Partner(s) Name: 
 *	Lab Section:022
 *	Assignment: Lab #12  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif

typedef struct task {
	signed char state; 
	unsigned long int period; 
	unsigned long int elapsedTime; 
	int (*TickFct)(int); 
} task;

unsigned short patterns[5] = {0xF0, 0x78, 0x3C, 0x1E, 0x0F};
unsigned short rows[3] = {0xFA, 0xF5, 0xEB};
unsigned short pattern2[5] = {0x90, 0x48, 0x24, 0x12, 0x09};
unsigned short row2[3] = {0xFD, 0xFB, 0xF7};

enum Rect_state {rect_0, rect_1};
int Rect_Tick(int state) {
	switch(state) {
		case rect_0:
			state = rect_1;
			break;
		case rect_1:
			state = rect_0;
			break;
		default:
			state = rect_0;
			break;
	}
	switch(state) {
		case rect_0:
			PORTC = patterns[2];
			PORTD = rows[1];
			break;
		case rect_1:
			PORTC = pattern2[2];
			PORTD = row2[1];
			break;
	}
	return state;
}

//find_GCD
unsigned long int findGCD(unsigned long int a, unsigned long int b){
        unsigned long int c;
        while (1){
                c = a%b;
                if (c == 0) {return b;}
                a = b;
                b = c;
        }
        return 0;
}

//find_GCD


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00;  	PORTA = 0xFF;
	DDRC = 0xFF;  	PORTC = 0x00;
	DDRD = 0xFF;  	PORTD = 0x00;
    /* Insert your solution below */
	
	static task task1;
	task *tasks[] = {&task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	tasks[0]->state = start;
	tasks[0]->period = 1;
	tasks[0]->elapsedTime = tasks[0]->period;
	tasks[0]->TickFct = &Rect_Tick;

	unsigned short i;
	unsigned long gcd = tasks[0]->period;
	for (i = 1; i < numTasks; i++){
		gcd = findGCD(gcd, tasks[i]->period);
	}

	TimerSet(gcd);
	TimerOn();

    while (1) {
	for(i = 0; i < numTasks; i++){
		if(tasks[i]->elapsedTime == tasks[i]->period){
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += gcd;
	}
	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 0;
}
