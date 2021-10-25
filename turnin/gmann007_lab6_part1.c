/*	Author: Gurvinder Mann
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab # 6 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
* Demo Video: My programming chip broke and they didn't have any at Solarium until monday
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerISR() { TimerFlag = 1;}

void TimerOff() {
	TCCR1B = 0x00;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States {Start,On_ Light1,On_ Light2,On_ Light3} state;

void Tick() {
	switch(state) {
		case Start:	state = On_Light1; break;
		case On_Light1:	state = On_Light2; break;
		case On_Light2:	state = On_Light3; break;
		case On_Light3: state = On_Light1; break;
		default:	state = Start; break;
	}
	switch (state) {
		case Start:                        break;
                case On_Light1:      PORTB = 0x01; break;
	        case On_Light2:      PORTB = 0x02; break;
		case On_Light3:      PORTB = 0x04; break;
		default:                           break;
	}
}

void main(void) {
    DDRB = 0xFF;
    PORTB = 0x00;
    TimerSet(1000);
    TimerOn();
    unsigned char tmpB = 0x00;
    while (1) {
	Tick();
	while(!TimerFlag) {};
	TimerFlag = 0;
    }
    return 1;
}
