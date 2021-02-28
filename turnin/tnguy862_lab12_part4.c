/*	Author: Tann Nguyen
 *  Partner(s) Name: N/A
 *	Lab Section: 022
 *	Assignment: Lab #12  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *		https://youtu.be/eI3qjzXjxK0
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
unsigned char pI = 2; 
unsigned char rI = 1; 

enum button_state {b_start, b_release, b_press};
int B_Tick(int state){
	unsigned char A0 = ~PINA & 0x01;
	unsigned char A1 = ~PINA & 0x02;
	unsigned char A2 = ~PINA & 0x04;
	unsigned char A3 = ~PINA & 0x08;
	
	switch (state) {
		case b_start:
			state = b_release;
			break;

		case b_release:
			if (A0 || A1 || A2 || A3){
				state = b_press;
			} else {
				state = b_release;
			}
			break;

		case b_press:
			if (A0 || A1 || A2 || A3){
				state = b_press;
			} else {
				state = b_release;
			}
			break;

		default:
			state = b_press;
			break;
	}
	
	switch (state) {
                case b_start:
                        break;

                case b_release:
                        break;

                case b_press:
			if (A0){ 
				if (rI == 2){
					rI = 2;
				} else {
					rI++;
				}
			} else if (A1){ 
				if (rI == 0){
					rI = 0;
				} else {
					rI--;
				}
			} else if (A2){ 
				if (pI == 0 ){
					pI = 0;
				} else {
					pI--;
				}
			} else if (A3) {
				if (pI == 4){
					pI = 4;
				} else {
					pI++;
				}
			}
                        break;
	}

	return state;
}

enum Rect_state {rect_0, rect_1};
int Rect_Tick(int state) {
	switch (state) {
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
	switch (state) {
		case rect_0:
			PORTC = patterns[pI];
			PORTD = rows[rI];
			break;
		
		case rect_1:
			PORTC = pattern2[pI];
			PORTD = row2[rI];
			break;

		default:
			break;
	}
	return state;
}

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



int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00;  PORTA = 0xFF;
	DDRC = 0xFF;  PORTC = 0x00;
	DDRD = 0xFF;  PORTD = 0x00;
    /* Insert your solution below */
	
	static task task1, task2;
	task *tasks[] = {&task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;


        tasks[0]->state = start;
        tasks[0]->period = 100;
        tasks[0]->elapsedTime = tasks[0]->period;
        tasks[0]->TickFct = &B_Tick;

	tasks[1]->state = start;
	tasks[1]->period = 1;
	tasks[1]->elapsedTime = tasks[1]->period;
	tasks[1]->TickFct = &Rect_Tick;

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

