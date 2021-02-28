/*	Author: Tann Nguyen
 *  Partner(s) Name: 
 *	Lab Section:022
 *	Assignment: Lab #12  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link:
 *	https://www.youtube.com/watch?v=r2N7Cm4vpxQ
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


enum Shift_States {shift_init, shift_n, shift_press, shift_rls};
int Shift_Tick(int state) {
	unsigned char A0 = ~PINA & 0x01;
	unsigned char A1 = ~PINA & 0x02;
	static unsigned char pattern = 0x80;
	static unsigned char row = 0x00;
	switch (state) {
		case shift_init:
			state = shift_n;	
			break;
		case shift_n:
			if(A0 || A1)
				state = shift_press;
			break;
		case shift_press:
			state = shift_rls;
			break;
		case shift_rls:
			if(A0 || A1)
				state = shift_rls;
			else
				state = shift_n;
			break;
		default:	
			state = shift_init;
			break;
	}	

	// Actions
	switch (state) {
		case shift_press:
			if(A0) {
				if(pattern == 0x80)
					pattern = 0x80;
				else
					pattern << 1;
			}
			else if (A1) {
				if(pattern == 0x01)
					pattern = 0x01;
				else
					pattern = pattern >> 1;
			}
			break;
		default:
			break;
	}
	PORTC = pattern;	// Pattern to display
	PORTD = row;		// Row(s) displaying pattern	
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
	tasks[0]->period = 100;
	tasks[0]->elapsedTime = tasks[0]->period;
	tasks[0]->TickFct = &Shift_Tick;

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
