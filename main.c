/*
 * Encoder.c
 *
 * Created: 29.08.2019 17:48:31
 * Author : GnevT34
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


volatile int state = 0;
volatile int pinA = 0;
volatile int pinB = 0;
volatile int pinA_last = 0;
volatile int pinB_last = 0;

ISR(PCINT0_vect)                              // function of interrupt on PINs 1 and 4
{
    pinA = PINB & 0b00000010;
    pinB = PINB & 0b00010000;
    cli();
    if (!pinA_last && !pinB_last)
    {
	if (!pinA && pinB)
	{
	    state = 1;
	}
	if (pinA && !pinB)
	{
    	    state = -1;
	}
    }
    if (pinA_last && pinB_last)
    {
	if (!pinA && pinB)
	{
	    state = -1;
	}
	if (pinA && !pinB)
	{
	    state = 1;
	}
    }
    if (state == 1)
    {
	PORTB |= (1<<0);
	_delay_ms(10);
	PORTB &= ~(1<<0);
	state = 0;
    }
    if (state == -1)
    {
	PORTB |= (1<<2);
	_delay_ms(10);
	PORTB &= ~(1<<2);
	state = 0;
    }
    pinA_last = pinA;
    pinB_last = pinB;
    sei();	
}

int main(void)
{ 
    GIMSK |= (1<<PCIE);                       // mask INT0 for new pins
    MCUSR |= (1<<ISC01);                      // change falling logical level 
    PCMSK |= (1<<PCINT1) | (1<<PCINT4);       // new pins (1,4) for external interrupt
    sei();
    DDRB = 0b00000101;
    PORTB = 0b00000000;
    pinA_last = PINB & 0b00000010;
    pinB_last = PINB & 0b00010000;
    while (1) 
    {

    }
}

