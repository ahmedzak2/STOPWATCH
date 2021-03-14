/*
 * project1.c
 *
 *  Created on: Mar 13, 2021
 *      Author: Ahmed
 *      date:13/3/2021
 *      description:Use Timer1 CTC Mode to count the time for the stop watch.
INT0(Falling edge using the internal pull up) --> reset the stop watch
INT1 (raising edge using the external pull down) --> pause the stop watch
INT2(Falling edge using the internal pull up) --> resume the stop watch.
 */

#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
unsigned char sec_one=0;
unsigned char sec_two=0;
unsigned char min_one=0;
unsigned char min_two=0;
unsigned char hours_one=0;
unsigned char hours_two=0;
void timer1_init_comp_mode( )
{
	TCCR1A=(1<<FOC1A)|(1<<FOC1B);//activate the force output compare for channel a and b
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS11); // pre scaler is FCPU/64 and activate the compare mood
	TIMSK|=(1<<OCIE1A);// Enable the interrupt for timer1 compare mode
	TCNT1=0;//Set Timer initial value to 0
	OCR1A=15625;//to count one second

}
void INT0_int(void)
{
	DDRD&=~(1<<PD2);
	PORTD |=(1<<PD2);
	GICR|=(1<<6);
	MCUCR=MCUCR|(1<<ISC01); // Enable failing edge

}
void INT1_int(void)
{
	DDRD= DDRD &(~(1<<PD3)); // configure pin 3 in port d as external interrupt
	GICR|=(1<<INT1); // Enable the general interrupt control register
	MCUCR |=(1<<ISC11); // Enable the MCU interrupt control register
	MCUCR =MCUCR & (~(1<<ISC10));

}
void INT2_int(void)
{
	DDRB= DDRB &(~(1<<PB2)); // configure pin 2 in port B as external interrupt
	GICR|=(1<<INT2); // Enable the general interrupt control register
	MCUCSR|=(1<<ISC2);

PORTB|=(1<<PB2);
}


ISR(TIMER1_COMPA_vect)
{
	sec_one++;
	if(sec_one==10)
	{
		sec_two++;//increase the the ten in sec
		sec_one =0;
	}
	if((sec_two==6)&&(sec_one==0))
	{

		sec_two=0;
		min_one++;
	}
	if(min_one==10)
	{
		min_one=0;
		min_two++;
	}
	if((min_two==6)&&(min_one==0))
	{

		min_two=0;
		hours_one++;
	}
	if(hours_one==10)
	{
		hours_one=0;
		hours_two ++;

	}
}
ISR(INT0_vect)
{
	sec_one=0;
	sec_two=0;
	min_one=0;
	min_two=0;
	hours_one=0;
	hours_two=0;
}
ISR(INT1_vect)
{
	TCCR1B&=~(1<<CS10);
		TCCR1B&=~(1<<CS11);
		TCCR1B&=~(1<<CS12);
}
ISR(INT2_vect)
{
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS11);
}

int main(void)
{
	SREG |= (1<<7); // Enable the global in interrupt
	timer1_init_comp_mode( );//initiate the timer 1
	DDRA|=0X3F;// make the first 6 pin is output to enable the 6 7-segment
	DDRC|=0X0F; // // configure pin 0,1,2,3 in port c for 7-segment
	PORTC&= 0xF0; // make the initial value is zero
	INT0_int();// Initiate the external interrupt 0
	INT1_int();// Initiate the external interrupt 1
	INT2_int();// Initiate the external interrupt 2
	while(1)
	{
		PORTA=(1<<0);// Enable the 1 (7-segment)
		PORTC= (PORTC & 0XF0) |(hours_two & 0X0F); // display the second hour
		_delay_ms(3);
		PORTA=(1<<1);// Enable the 2 (7-segment)
		PORTC= (PORTC & 0XF0) |(hours_one & 0X0F); // display the first hour
		_delay_ms(3);
		PORTA=(1<<2);// Enable the 3 (7-segment)
		PORTC= (PORTC & 0XF0) |(min_two & 0X0F); // display the second minute
		_delay_ms(3);
		PORTA=(1<<3);// Enable the 4 (7-segment)
		PORTC= (PORTC & 0XF0) |(min_one & 0X0F); // display the first minute
		_delay_ms(3);
		PORTA=(1<<4);// Enable the 5 (7-segment)
		PORTC= (PORTC & 0XF0) |(sec_two & 0X0F); // display the second second
		_delay_ms(3);
		PORTA=(1<<5);// Enable the 6 (7-segment)
		PORTC= (PORTC & 0XF0) |(sec_one & 0X0F); // display the first second
		_delay_ms(3);


	}
}
