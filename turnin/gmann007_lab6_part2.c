/*	Author: Gurvinder Mann
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab # 6 Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
* Demo Video: My programming chip broke and they don't have any at Solarium until monday
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned char Res = 0x03;

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

enum States {Start,On_Light1,On_Light2,On_Light3,Press,Wait} state;

void Tick() {
	switch(state) {
		case Start:	state = On_Light1; break;
		case On_Light1:
			if((~PINA & 0x01) == 0x01) {
				state = Press; break;
			}
			else
			{
				state = On_Light2; break;
			}
		case On_Light2:	
			if((~PINA & 0x01) == 0x01) {
			state = Press; break;
			}
			else
			{
				state = On_Light3; break;
			}
		case On_Light3:
	          	if((~PINA & 0x01) == 0x01) {	
			state = Press; break;
			}
			else
			{
				state = On_Light1; break;
			}
		case Press:
			if((~PINA & 0x01) == 0x00)
			{
				state = Wait; break;
			}
			else
			{
				state = Press; break;
			}
		case Wait:
			if((~PINA & 0x01) == 0x01)
			{
				if (Res == 0x00)
				{
					state = On_Light1; break;
				}
				else if (Res == 0x01)
				{
					state = On_Light1; break;
				}
				else if( Res == 0x02)
				{
					state = On_Light1; break;
				}
			}
			else
			{
				state = Wait; break;
			}

		default:	state = Start; break;
	}
	switch (state) {
		case Start:          PORTB = 0x00;  break;
                case On_Light1:      PORTB = 0x01; Res = 0x00; break;
	        case On_Light2:      PORTB = 0x02; Res = 0x01; break;
		case On_Light3:      PORTB = 0x04; Res = 0x02; break;
		case Press:          break;
		case Wait:           if((~PINA & 0x01) == 0x01)
				     {
					     break;
				     }
				     else
				     {
					     if(Res == 0x00)
					     {
						     PORTB = 0x01; break;
					     }
					     else if(Res == 0x01) 
					     {
						     PORTB = 0x02; break;
					     }
					     else if(Res == 0x02)
					     {
						     PORTB = 0x04; break;
					     }

		default:                           break;
				     }
	}

}

int main(void) {
    DDRB = 0xFF;
    PORTB = 0x00;
    DDRA = 0xFF;
    PORTA = 0x00;
    TimerSet(300);
    TimerOn();
    while (1) {
	Tick();
	while(!TimerFlag) {};
	TimerFlag = 0;
    }
   
}
