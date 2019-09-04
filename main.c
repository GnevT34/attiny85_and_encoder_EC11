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


ISR(PCINT0_vect)                              // function of interrupt on PINs 1 and 4
{
	pinA = PINB & 0b00000010;
	pinB = PINB & 0b00010000;
	
	cli();
	
	if ((state == 0 && !pinA && pinB) || (state == 2 && pinA && !pinB))
	{
		state += 1;
	}
	
	if ((state == -1 && !pinA && !pinB) || (state == -3 && pinA && pinB))
	{
		state -= 1;
	}

	if ((state == 1 && !pinA && !pinB) || (state == 3 && pinA && pinB))
	{
		state += 1;
	}
	if ((state == 0 && pinA && !pinB) || (state == -2 && !pinA && pinB))
	{
		state -= 1;
	}

	if (state == 4)
	{
		PORTB |= (1<<0);
		_delay_ms(10);
		PORTB &= ~(1<<0);
	}
	if (state == -4)
	{
		PORTB |= (1<<2);
		_delay_ms(10);
		PORTB &= ~(1<<2);
	}
	sei();
	
	if (pinA && pinB && state != 0) state = 0;

}

int main(void)
{ 
//	TCCR1 = 0b00001011;
//	TCNT1 = 0b00000001;
	
	GIMSK |= (1<<PCIE);                       // mask INT0 for new pins
    MCUSR |= (1<<ISC01);                      // change falling logical level 
	PCMSK |= (1<<PCINT1) | (1<<PCINT4);       // new pins (1,4) for external interrupt
    sei();
	DDRB = 0b00000001;
	PORTB = 0b00011010;
    while (1) 
    {

    }
}

