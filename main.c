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


volatile int state = 0;                      // value of action
volatile int pinA = 0;                       // value pinA or CLK pin
volatile int pinB = 0;                       // value pinB or DT pin
volatile int pinS = 0;                       // value pinS or SW pin
volatile int pinA_last = 0;                  // last value pinA or CLK pin
volatile int pinB_last = 0;                  // last value pinA or CLK pin
volatile int turned = 0;                     // value of turn action
int pressed = 0;                             // value of press action
int timer1 = 0;                              // value for timer 1
int timer2 = 0;                              // value for timer 2
int longPressed = 0;                         // long press action's value
int checkDouble = 0;                         // value for check double click action


ISR(PCINT0_vect)                              // function of interrupt on PINs 1 and 4
{
	pinA = PINB & 0b00000010;                 // read pinA value (CLK)
	pinB = PINB & 0b00010000;                 // read pinB value (DT)
	pinS = PINB & 0b00001000;                 // real pinS value (SW)
	cli();                                    // turn off extern interrupts
	if (!pinA_last && !pinB_last)             // if last values CLK and DT were 0
	{
		if (!pinA && pinB)                    // if values CLK and DT are 01
		{
			if (pinS) state = 1;              // if key is not pressed
			if (!pinS) state = 2;             // if key is pressed
		}
		if (pinA && !pinB)                    // if values CLK and DT are 10
		{
			if (pinS) state = -1;             // if key is not pressed
			if (!pinS) state = -2;            // if key is pressed
		}
	}
	if (pinA_last && pinB_last)               // if last values CLK and DT were 1
	{
		if (!pinA && pinB)                    // if values CLK and DT are 01
		{
			if (pinS) state = -1;             // if key is not pressed
			if (!pinS) state = -2;            // if key is pressed
		}
		if (pinA && !pinB)                    // if values CLK and DT are 10
		{
			if (pinS) state = 1;              // if key is not pressed
			if (!pinS) state = 2;             // if key is pressed
		}
	}
	

/************************************************************************/
/* Action by states.                                                    */
/* For example I use combinations of LED's blinking on pins PB0 and PB2 */
/************************************************************************/
	
	if (state == 1)                           // when turn to right
	{
		PORTB |= (1<<0);
		_delay_ms(25);
		PORTB &= ~(1<<0);
		state = 0;
	}
	if (state == -1)                           // when turn to left
	{
		PORTB |= (1<<2);
		_delay_ms(25);
		PORTB &= ~(1<<2);
		state = 0;
	}
	if (state == 2)                           // when turn to right with key's pressed
	{
		PORTB |= (1<<0);
		_delay_ms(250);
		PORTB &= ~(1<<0);
		state = 0;
	}
	if (state == -2)                           // when turn to left with key's pressed
	{
		PORTB |= (1<<2);
		_delay_ms(250);
		PORTB &= ~(1<<2);
		state = 0;
	}	
	pinA_last = pinA;                         // save last pinA value
	pinB_last = pinB;                         // save last pinB value
	turned = 1;                               // turn on flag for turn's action
	sei();                                    // turn on all extern interrupts
/************************************************************************/
}

int main(void)
{
	TCCR1 = 0b00001011;                       // setting for timer1
	TCNT1 = 0b00000001;                       // reset timer 1
	GIMSK |= (1<<PCIE);                       // mask INT0 for new pins
  	MCUSR |= (1<<ISC01);                      // change falling logical level 
	PCMSK |= (1<<PCINT1) | (1<<PCINT4);       // new pins (1,4) for external interrupts
   	sei();                                    // turn on interrupts
	DDRB = 0b00000101;                        // setup input and output pins
	PORTB = 0b00001000;                       // setup pull-up 
	pinA_last = PINB & 0b00000010;            // read first value pinA
  	pinB_last = PINB & 0b00010000;            // read first value pinB
    while (1) 
    {
		pinS = PINB & 0b00001000;             // read pinS (SW) value 
		if (!pinS)                            // if key is pressed
		{		
			pressed = 1;                      // turn on flag for pressed key's action
			if (checkDouble) checkDouble = 2;   // check flag of first click for double click action
			if (!turned)                      // check turned actions
			{
				if (TCNT1 == 255) {timer1 ++; TCNT1 = 0;}   // start timer1 for long press action
				if (timer1 > 40)                            // stop timer, activate LED's blinking action
				{
					PORTB |= (1<<0);                        
					PORTB |= (1<<2);
					_delay_ms(250);
					PORTB &= ~(1<<0);
					PORTB &= ~(1<<2);
					pressed = 0;               // reset flag of pressed action
					timer2 = 0;                // reset timer2
					timer1 = 0;                // reset timer1
					longPressed = 1;           // turn on flag of long press action
					checkDouble = 0;           // reset double click flag
				}			
			}						
		}
		if (pinS && pressed)                   // check state when pinS (SW) was release
		{
			if (turned || longPressed)         // check turn and long press flags
			{
				timer2 = 0;                    // reset timer2
				timer1 = 0;                    // reset timer1
				longPressed = 0;               // reset long press flag
				turned = 0;                    // reset turn flag
				pressed = 0;                   // reset press flag
			}
			else                               // if key was release but press flag is active
			{
				if (timer1 < 20 && !checkDouble) checkDouble = 1;   // check value of timer1 and first press flag of double click 
				if (TCNT1 == 255) {timer2 ++; TCNT1 = 0;}   // start timer2 for double click action
				if (timer2 > 20 && checkDouble)   // check for one click action. if timer's value more than 20 activate one click LED action 
				{
					PORTB |= (1<<0);
					PORTB |= (1<<2);
					_delay_ms(500);
					PORTB &= ~(1<<0);
					PORTB &= ~(1<<2);
					timer2 = 0;                 // reset timer2
					timer1 = 0;                 // reset timer1
					pressed = 0;                // reset press flag
					checkDouble	= 0;            // reset double click flag
				}
				if (checkDouble == 2)           // check double click action. if flag of double click's value is 2 activate double click LED action
				{	
					PORTB |= (1<<0);
					PORTB |= (1<<2);
					_delay_ms(100);
					PORTB &= ~(1<<0);
					PORTB &= ~(1<<2);
					_delay_ms(100);
					PORTB |= (1<<0);
					PORTB |= (1<<2);
					_delay_ms(100);
					PORTB &= ~(1<<0);
					PORTB &= ~(1<<2);
					timer2 = 0;                 // reset timer2
					timer1 = 0;                 // reset timer1
					pressed = 0;                // reset press flag
					checkDouble = 0;            // reset double click flag
				}
			}
		}
         }
}
