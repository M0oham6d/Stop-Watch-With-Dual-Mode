/*
 * Stopwatch_With_Dual_Mode.c
 *
 *  Created on: Sep 9, 2024
 *      Author: Muhammed Ayman
 *
 *      Stop Watch With Dual Mode Project
 */

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#define DELAY_7SEGMETS 2					/* Delay to keep others turned on. */
#define I_BIT 7								/* For I bit. */

unsigned char seconds_counter0 = 0;			/* First 7 segment counter for seconds. */
unsigned char seconds_counter1 = 0;			/* Second 7 segment counter for seconds. */
unsigned char minutes_counter0 = 0;			/* First 7 segment counter for minutes. */
unsigned char minutes_counter1 = 0;			/* Second 7 segment counter for minutes. */
unsigned char hours_counter0 = 0;			/* First 7 segment counter for hours. */
unsigned char hours_counter1 = 0;			/* Second 7 segment counter for hours. */

unsigned char increment_hours_flag = 1;		/* Hours button for incrementing with one click. */
unsigned char decrement_hours_flag = 1;		/* Hours button for decrementing with one click. */
unsigned char increment_minutes_flag = 1;	/* Minutes button for incrementing with one click. */
unsigned char decrement_minutes_flag = 1;	/* Minutes button for decrementing with one click. */
unsigned char increment_seconds_flag = 1;	/* Seconds button for incrementing with one click. */
unsigned char decrement_seconds_flag = 1;	/* Seconds button for decrementing with one click. */

unsigned char mode_flag = 0;				/* For selecting mode(increment, decrement). */

/*
 * Increasing counters based on timer1.
 * If it reach 1 minute the seconds counters will be reset and increase the first minute counter.
 * If it reach 1 hour the minutes counter and seconds counters will be reset and increase the first hour counter.
 * If it reach 23:59:59, it will be reset and start counting from 00:00:00.
 */
ISR(TIMER1_COMPA_vect)
{
	SREG |= 1<<I_BIT;				/* Enabling I bit. */

	/* If count down mode on. */
	if(mode_flag)
	{
		/* If it reach 00:00:00, buzzer will be on. */
		if(0 == hours_counter1 && 0 == hours_counter0 &&\
				0 == minutes_counter1 && 0 == minutes_counter0 &&\
				0 == seconds_counter1 && 0 == seconds_counter0)
		{
			PORTD |= 1<<PD0;		/* Turn on buzzer if it reach 00:00:00. */
		}

		/* Else it count down until zero. */
		else
		{
			PORTD &= ~(1<<PD0);		/* Turn off buzzer. */

			/* If the first seconds seconds 7 segment reach to 0, it will reset to 9 and decrement again. */
			if(0 == seconds_counter0)
			{
				seconds_counter0 = 9;
				/* If the second seconds 7 segment reach to 0, it will reset to 5 and decrement again. */
				if(0 == seconds_counter1)
				{
					seconds_counter1 = 5;
					/* If the first minutes 7 segment reach to 0, it will reset to 9 and decrement again. */
					if(0 == minutes_counter0)
					{
						minutes_counter0 = 9;
						/* If the second minutes 7 segment reach to 0, it will reset to 5 and decrement again. */
						if(0 == minutes_counter1)
						{
							minutes_counter1 = 5;
							/* If the first hours 7 segment reach to 0, it will reset to 9 and decrement again. */
							if(0 == hours_counter0)
							{
								hours_counter0 = 9;
								/* If the second hours 7 segment reach to 0, it will stay at 0 and decrement again. */
								if(0 == hours_counter1)
								{
									hours_counter1 = 0;
								}
								else
								{
									hours_counter1--;
								}
							}
							else
							{
								hours_counter0--;
							}
						}
						else
						{
							minutes_counter1--;
						}
					}
					else
					{
						minutes_counter0--;
					}
				}
				else
				{
					seconds_counter1--;
				}
			}
			else
			{
				seconds_counter0--;
			}
		}
	}

	/* If count up mode is on, it will count until it reach 23:59:59 then reset. */
	else
	{
		PORTD &= ~(1<<PD0);			/* Turn off the buzzer. */

		/* If stop watch reach 23:59:59 which is 24 hour, it will reset. */
		if(2 == hours_counter1 && 3 == hours_counter0 && \
				5 == minutes_counter1 && 9 == minutes_counter0 && \
				5 == seconds_counter1 && 9 == seconds_counter0)
		{
			seconds_counter0 = 0;
			seconds_counter1 = 0;
			minutes_counter0 = 0;
			minutes_counter1 = 0;
			hours_counter0 = 0;
			hours_counter1 = 0;
		}
		else
		{
			seconds_counter0++;		/* Increasing 1 second in the first 7 segment. */
			/* If the first seconds 7 segment reach 10, it will reset and increase the second seconds 7 segment. */
			if(10 == seconds_counter0)
			{
				seconds_counter0 = 0;
				seconds_counter1++;
				/* If the second seconds 7 segment reach 6 which is 1 minute, it will reset and increase the first minutes 7 segment. */
				if(6 == seconds_counter1)
				{
					seconds_counter1 = 0;
					minutes_counter0++;
					/* If the first minutes 7 segment reach 10, it will reset and increase the second minutes 7 segment. */
					if(10 == minutes_counter0)
					{
						minutes_counter0 = 0;
						minutes_counter1++;
						/* If the second minutes 7 segment reach 6 which is 1 hour, it will reset and increase the first hours 7 segment. */
						if(6 == minutes_counter1)
						{
							minutes_counter1 = 0;
							hours_counter0++;
							/* If the first hours 7 segment reach 10, it will reset and increase the seconds hours 7 segment. */
							if(10 == hours_counter0)
							{
								hours_counter0 = 0;
								hours_counter1++;
							}
						}
					}
				}
			}
		}
	}
}

/* For resetting stop watch when falling edge detected. */
ISR(INT0_vect)
{
	SREG |= 1<<I_BIT;			/* Enabling I bit. */

	/* Resetting all 7 segments. */
	seconds_counter0 = 0;
	seconds_counter1 = 0;
	minutes_counter0 = 0;
	minutes_counter1 = 0;
	hours_counter0 = 0;
	hours_counter1 = 0;
}

/* For pause stop watch when raising edge detected. */
ISR(INT1_vect)
{
	SREG |= 1<<I_BIT;	/* Enabling I bit. */

	/* Disable timer clock --> CS12 = 0, CS11 = 0, CS10 = 0. */
	TCCR1B &= ~(1<<CS12) & ~(1<<CS11) & ~(1<<CS10);
}

/* For resume stop watch when falling edge detected. */
ISR(INT2_vect)
{
	SREG |= 1<<I_BIT;	/* Enabling I bit. */

	/* Enable timer clock --> CS12 = 0, CS10 = 0. */
	TCCR1B |= (1<<CS10) | (1<<CS12);
}

/* Initializing timer1. */
void timre1_init(void)
{
	/*
	 * FOC1A --> Non PWM
	 * WGM12 --> CTC
	 * CS10 & CS12 --> CLK/1024
	 */
	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
	TCNT1 = 0;			/* Initial counter register with zero. */
	OCR1A = 15625;		/* Making interrupt in 1s */
	TIMSK |= 1<<OCIE1A;	/* Enabling interrupt module. */
}

/* Initializing INT0 */
void int0_reset_init(void)
{
	/*
	 * Falling edge --> ISC00 = 0, ISC01 = 1.
	 * Enabling module interrupt --> INT0.
	 */
	MCUCR |= 1<<ISC01;
	GICR |= 1<<INT0;
}

/* Initializing INT1 */
void int1_pause_init(void)
{
	/*
	 * Raising edge --> ISC10 = 1, ISC11 = 1.
	 * Enabling module interrupt --> INT1.
	 */
	MCUCR |= (1<<ISC10) | (1<<ISC11);
	GICR |= 1<<INT1;
}

/* Initializing INT2 */
void int2_resume_init(void)
{
	/*
	 * Falling edge --> ISC2 = 0.
	 * Enabling module interrupt --> INT2.
	 */
	GICR |= 1<<INT2;
}

/* For incrementing hours using button. */
void increment_hours(void)
{
	if(!(PINB & (1<<PB1)))
	{
		_delay_ms(30);				/* For de-bouncing. */
		if(!((PINB & (1<<PB1))) && (increment_hours_flag))
		{
			increment_hours_flag = 0;		/* Resetting flag to increment with one click only. */
			/*
			 * Increase the first hours 7 segment,
			 * If it reach 24 it will reset.
			 */
			hours_counter0++;
			if(4 == hours_counter0 && 2 == hours_counter1)
			{
				hours_counter0 = 0;
				hours_counter1 = 0;
			}
			else if(10 == hours_counter0)
			{
				hours_counter0 = 0;
				hours_counter1++;
			}
		}
	}
	else
	{
		increment_hours_flag = 1;		/* Resetting flag into 1 if push button released. */
	}
}

/* For decrementing hours using button. */
void decrement_hours(void)
{
	if(!(PINB & (1<<PB0)))
	{
		_delay_ms(30);				/* For de-bouncing. */
		if(!(PINB & (1<<PB0)) && (decrement_hours_flag))
		{
			decrement_hours_flag = 0;		/* Resetting flag to increment with one click only. */
			/*
			 * If hours reach zero, go back to 23.
			 * If the first hours 7 segment reach 0 it will reset to 9 and decrement the next hours 7 segment.
			 * Else, it is decrementing the first hours 7 segment.
			 */
			if(0 == hours_counter1 && 0 == hours_counter0)
			{
				hours_counter0 = 3;
				hours_counter1 = 2;
			}
			else if(0 == hours_counter0)
			{
				hours_counter0 = 9;
				hours_counter1--;
			}
			else
			{
				hours_counter0--;
			}
		}
	}
	else
	{
		decrement_hours_flag = 1;		/* Resetting flag into 1 if push button released. */
	}
}

/* For incrementing minutes using button. */
void increment_minutes(void)
{
	if(!(PINB & (1<<PB4)))
	{
		_delay_ms(30);				/* For de-bouncing. */
		if(!((PINB & (1<<PB4))) && (increment_minutes_flag))
		{
			increment_minutes_flag = 0;		/* Resetting flag to increment with one click only. */
			/*
			 * Increase the first minutes 7 segment.
			 * If it reach 59 it will reset.
			 */
			minutes_counter0++;
			if(10 == minutes_counter0)
			{
				minutes_counter0 = 0;
				minutes_counter1++;
				if(6 == minutes_counter1)
				{
					minutes_counter1 = 0;
				}
			}
		}
	}
	else
	{
		increment_minutes_flag = 1;		/* Resetting flag into 1 if push button released. */
	}
}

/* For decrementing minutes using button. */
void decrement_minutes(void)
{
	if(!(PINB & (1<<PB3)))
	{
		_delay_ms(30);				/* For de-bouncing. */
		if(!(PINB & (1<<PB3)) && (decrement_minutes_flag))
		{
			decrement_minutes_flag = 0;		/* Resetting flag to increment with one click only. */
			/*
			 * If minute reach zero, do nothing.
			 * If the first minutes 7 segment reach 0 it will reset to 9 and decrement the next minutes 7 segment.
			 * Else, it is decrementing the first minutes 7 segment.
			 */
			if(0 == minutes_counter1 && 0 == minutes_counter0)
			{
				minutes_counter0 = 9;
				minutes_counter1 = 5;
			}
			else if(0 == minutes_counter0)
			{
				minutes_counter0 = 9;
				minutes_counter1--;
			}
			else
			{
				minutes_counter0--;
			}
		}
	}
	else
	{
		decrement_minutes_flag = 1;		/* Resetting flag into 1 if push button released. */
	}
}

/* For incrementing seconds using button. */
void increment_seconds(void)
{
	if(!(PINB & (1<<PB6)))
	{
		_delay_ms(30);				/* For de-bouncing. */
		if(!((PINB & (1<<PB6))) && (increment_seconds_flag))
		{
			increment_seconds_flag = 0;		/* Resetting flag to increment with one click only. */
			/*
			 * Increase the first minutes 7 segment.
			 * if it reach 59 it will reset.
			 */
			seconds_counter0++;
			if(10 == seconds_counter0)
			{
				seconds_counter0 = 0;
				seconds_counter1++;
				if(6 == seconds_counter1)
				{
					seconds_counter1 = 0;
				}
			}
		}
	}
	else
	{
		increment_seconds_flag = 1;		/* Resetting flag into 1 if push button released. */
	}
}

/* For decrementing minutes using button. */
void decrement_seconds(void)
{
	if(!(PINB & (1<<PB5)))
	{
		_delay_ms(30);				/* For de-bouncing. */
		if(!(PINB & (1<<PB5)) && (decrement_seconds_flag))
		{
			decrement_seconds_flag = 0;		/* Resetting flag to increment with one click only. */
			/*
			 * If minute reach zero, do nothing.
			 * If the first minutes 7 segment reach 0 it will reset to 9 and decrement the second minutes 7 segment.
			 * Else, it is decrementing the first minutes 7 segment.
			 */
			if(0 == seconds_counter1 && 0 == seconds_counter0)
			{
				seconds_counter0 = 9;
				seconds_counter1 = 5;
			}
			else if(0 == seconds_counter0)
			{
				seconds_counter0 = 9;
				seconds_counter1--;
			}
			else
			{
				seconds_counter0--;
			}
		}
	}
	else
	{
		decrement_seconds_flag = 1;		/* Resetting flag into 1 if push button released. */
	}
}

/* For toggle the mode and the 2 LEDs */
void mode_toggle(void)
{
	if(!(PINB & (1<<PB7)))
	{
		_delay_ms(30);
		if(!(PINB & (1<<PB7)))
		{
			mode_flag = 1;			/* Set flag to enable decrement mode. */
			PORTD |= 1<<PD5;		/* Turn ON increment LED. */
			PORTD &= ~(1<<PD4);		/* Turn OFF decrement LED. */
		}
	}
	else
	{
		mode_flag = 0;				/* Reset flag to enable decrement mode. */
		PORTD &= ~(1<<PD5);			/* Turn OFF increment LED. */
		PORTD |= 1<<PD4;			/* Turn OFF decrement LED. */
	}
}

/* For keeping 7 segments on all time. */
void lightning_7segments_in_parallel(void)
{
	PORTA = (PORTA & 0xC0) | 1<<PA5;				/* Open the 7 segment for secounds_counter0. */
	PORTC = (PORTC & 0xF0) | seconds_counter0;		/* Increasing the counter and display it. */
	_delay_ms(DELAY_7SEGMETS);						/* Delay to keep other turned on. */

	PORTA = (PORTA & 0xC0) | 1<<PA4;				/* Open the 7 segment for secounds_counter1. */
	PORTC = (PORTC & 0xF0) | seconds_counter1;		/* Increasing the counter and display it. */
	_delay_ms(DELAY_7SEGMETS);						/* Delay to keep other turned on. */

	PORTA = (PORTA & 0xC0) | 1<<PA3;				/* Open the 7 segment for minutes_counter0. */
	PORTC = (PORTC & 0xF0) | minutes_counter0;		/* Increasing the counter and display it. */
	_delay_ms(DELAY_7SEGMETS);						/* Delay to keep other turned on. */

	PORTA = (PORTA & 0xC0) | 1<<PA2;				/* Open the 7 segment for minutes_counter1. */
	PORTC = (PORTC & 0xF0) | minutes_counter1;		/* Increasing the counter and display it. */
	_delay_ms(DELAY_7SEGMETS);						/* Delay to keep other turned on. */

	PORTA = (PORTA & 0xC0) | 1<<PA1;				/* Open the 7 segment for hours_counter0. */
	PORTC = (PORTC & 0xF0) | hours_counter0;		/* Increasing the counter and display it. */
	_delay_ms(DELAY_7SEGMETS);						/* Delay to keep other turned on. */

	PORTA = (PORTA & 0xC0) | 1<<PA0;				/* Open the 7 segment for hours_counter. */
	PORTC = (PORTC & 0xF0) | hours_counter1;		/* Increasing the counter and display it. */
	_delay_ms(DELAY_7SEGMETS);						/* Delay to keep other turned on. */
}

int main(void)
{
	SREG |= 1<<I_BIT;	/* Enabling I bit. */

	timre1_init();		/* Turn on timer1 as time counter. */
	int0_reset_init();	/* Turn in INT0 with falling edge. */
	int1_pause_init();	/* Turn in INT1 with raising edge. */
	int2_resume_init();	/* Turn in INT2 with falling edge. */

	DDRA |= 0x3F;		/* Making first 6 pins in PORTA output for turning on 7 segments. */
	PORTA &= 0xC0;		/* It will out zero in the beginning. */

	DDRB = 0x00;		/* Making all pins in PORTB input for buttons. */
	PORTB = 0xFF;		/* Enabling internal pull up resistor. */

	DDRC |= 0x0F;		/* Making first 4 pins in PORTC output pins for counting. */
	PORTC &= 0xF0;		/* It will out zero in the beginning. */

	/*
	 * Making PD0 output pin for buzzer.
	 * Making PD2 and PD3 input pins for RESET and PAUSE and enable internal pull up resistance for PD3.
	 * Making PD4 and PD5 output pins for 2 mode LEDs.
	 */
	DDRD = 0x31;
	PORTD = 0x14;

	while(1)
	{
		lightning_7segments_in_parallel();		/* Displaying numbers on all 7 segments always. */
		increment_hours();						/* For incrementing hours using button. */
		decrement_hours();						/* For decrementing hours using button. */
		increment_minutes();					/* For incrementing minutes using button. */
		decrement_minutes();					/* For decrementing minutes using button. */
		increment_seconds();					/* For incrementing seconds using button. */
		decrement_seconds();					/* For decrementing seconds using button. */
		mode_toggle();							/* For toggling mode using button */
	}

	return 0;
}
