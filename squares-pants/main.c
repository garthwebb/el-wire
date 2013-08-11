/*
    EL Driver test code
	Pete Dokter, 5/20/09
	
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define EL_G 0	//PB0
#define EL_H 1	//PB1

#define EL_A 0	//PC0
#define EL_B 1	//PC1
#define EL_C 2	//PC2
#define EL_D 3	//PC3
#define EL_E 4	//PC4
#define EL_F 5	//PC5


//Define functions
//======================
void ioinit(void);      //Initializes IO
void delay_ms(uint16_t x); //General purpose delay
void delay_us(int x);

void pulse(char line, int speed);
void cycle(int speed);
void line_on(char line);//send 'A' through 'H'
void line_off(char line);//send 'A' through 'H'

//======================

static char line_on_1 = 0;
static char line_on_2 = 0;


int main (void)
{
	int x, slow, delay, count;

    ioinit(); //Setup IO pins and defaults
    
	delay_ms(5000);
	slow = 5;
	delay = 0;

	while (1) {
		count = 5;
		while (count > 0) {
			for (x = 65; x <= 72; x++) {
				line_off(x);
				delay_ms(delay);
				line_on(x);
			
				delay += slow;
				if (delay <= 0) {
					delay = 0;
					slow *= -1;
				} else if (delay > 200) {
					delay = 200;
					slow *= -1;
				}
			}
			count -= 1;
		}
	
	
		count = 5;
		while (count > 0) {
			for (x = 72; x >= 65; x--) {
				line_off(x);
				delay_ms(delay);
				line_on(x);
			
				delay += slow;
				if (delay <= 0) {
					delay = 0;
					slow *= -1;
				} else if (delay > 200) {
					delay = 200;
					slow *= -1;
				}
			}

			count -=1;
		}
	}
}

void ioinit (void)
{
	//PORTB = 0;
	PORTB = 255;
	DDRB = 0x03;
	
	PORTC = 0;
	DDRC = 0;

	//PORTD = 0;
	PORTD = 255;
	DDRD = 0xFC;

    TCCR2B = (1<<CS21); //Set Prescaler to 8. CS21=1

}

//General short delays
void delay_us(int x)
{
	int y, z, a;
	
	y = x/256;
	z = x - y * 256;
	
	for (a = 0; a < y; a++)
	{
		TIFR2 |= 0x01;//Clear any interrupt flags on Timer2
		
		TCNT2 = 0; //256 - 125 = 131 : Preload timer 2 for x clicks. Should be 1us per click
	
		while(!(TIFR2 & 0x01));
		
	}
	
	TIFR2 |= 0x01;//Clear any interrupt flags on Timer2
	
	TCNT2 = 256-z; //256 - 125 = 131 : Preload timer 2 for x clicks. Should be 1us per click

	while(!(TIFR2 & 0x01));
	
}

//General short delays
void delay_ms(uint16_t x)
{
	for (; x > 0 ; x--)
    {
        delay_us(250);
        delay_us(250);
        delay_us(250);
        delay_us(250);
    }
	
}


void pulse(char line, int speed)
{
	int x;
	
	for (x = 0; x < 10000; x+=speed)
	{
		line_on(line);
		delay_us(x + 100);
		line_off(line);
		delay_us(10000 - x);
	}

	for (x = 0; x < 10000; x+=speed)
	{
		line_on(line);
		delay_us(10100 - x);
		line_off(line);
		delay_us(x + 100);
	}
}

/*
void fade_switch (char line_a, char line_b, int speed) {
	int x;
	
	for (x = 0; x < 10000; x+=speed)
	{
		line_on(line_a);
		delay_us(x + 100);
		line_off(line_a);
		delay_us(10000 - x);
	}

	for (x = 0; x < 10000; x+=speed)
	{
		line_on(line);
		delay_us(10100 - x);
		line_off(line);
		delay_us(x + 100);
	}

}
*/

void line_on(char line)//send 'A' through 'H'
{
	char temp;
	
//	if (line_on_2 != 0) line_off(line_on_2);//can't have more than one line on at a time
	
	//keep track of what's on and in what sequence
	line_on_2 = line_on_1;
	line_on_1 = line;

	temp = line - 65;
	
	if (temp < 6) PORTD |= (1<<(temp+2));
	else PORTB |= (1<<(temp-6));
        
}


void line_off(char line)//send 'A' through 'H'
{
	char temp;
	
	if (line == line_on_2) line_on_2 = 0;
	else if (line == line_on_1)
	{
		line_on_1 = line_on_2;
		line_on_2 = 0;
	}
	
	temp = line - 65;
	
	if (temp < 6) PORTD &= ~(1<<(temp+2));
	else PORTB &= ~(1<<(temp-6));
}

void cycle(int speed)
{
	int x;
	
	if (speed > 32000) speed = 32000;
	
	for (x = 65; x < 72; x++)
	{
		line_on(x);
		delay_us(32100 - speed);
	}
	
	for (x = 72; x >= 65; x--)
	{
		line_on(x);
		delay_us(32100 - speed);
	}
	
	line_off('B');
	delay_us(32100 - speed);
	line_off('A');
}



