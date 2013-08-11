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

void line_on(int line);//send 'A' through 'H'
void line_off(int line);//send 'A' through 'H'

void popcorn(int num);
void powersuit(int num);
void light_stream(int num);

//======================

int main (void)
{
    ioinit(); //Setup IO pins and defaults
    
	delay_ms(5000);

	while (1) {
		powersuit(250);
		popcorn(80);
		light_stream(500);
	}
}

void powersuit (int num) {
	int x, cycle, count, speed, max_val, cur_val, step;
	int light[8] = {224, 192, 160, 128, 96, 64, 32, 0};

	max_val = 224;
	cur_val = 224;
	step = 8;
	speed = 4;
	cycle = 0;
    count = 0; // PWM counter

	while (num) {
	
		// Light the wires appropriately
		for (x = 0; x <=7; x++) {
		    if (light[x] >= count) {
			    line_on(x);
			} else {
				line_off(x);
			}
		}
		count++;
		if (count >= max_val) {
		    count = 0;
			cycle++;
		}
		
		if (cycle == speed) {
			cur_val += step;
			if (cur_val >= max_val) {
				step *= -1;
				cur_val = max_val;
			}
		
			if (cur_val <= 0) {
				step *= -1;
				cur_val = 0;
			}
		
			for (x = 0; x <= 7; x++) {
				light[x] = cur_val;
			}

			cycle = 0;
			num--;
		}
	}
}

void popcorn (int num) {
	int x;
	
	while (num--) {
		for (x = 0; x <=7; x++) {
			line_on(x);
			delay_ms(50);
			line_off(x);
			delay_ms(50);
		}
	}
}

void light_stream (int num) {
	int x, cycle, count, max_val, max_step, cur_spot, direction;
	int brake, speed, max_speed, min_speed;
	int next_switch;

	int light[8] = {0, 0, 0, 0, 0, 0, 0, 0};


	max_val = 224;
	max_step = 32;
	cur_spot = 0;
	direction = 1;

	// NOTE: For 'speed', fast == smaller numbers, slow == larger numbers
	speed = 4; // This determines how many cycles have to pass before we update
	brake = 0; // When the brakes go on, start slowing down
	min_speed = 10; // When speed hits this, reverse direction
	max_speed = 4;

	next_switch = 150; // How many cycles before switching directions

	cycle = 0;
    count = 0;
	
	while (num) {
	
		// Light the wires apropriately
		for (x = 0; x <=7; x++) {
		    if (light[x] >= count) {
			    line_on(x);
			} else {
				line_off(x);
			}
		}
		count++;
		if (count >= max_val) {
		    count = 0;
			cycle++;
		}
		
		// Every set number of cycles, update 
		if (cycle == speed) {
			for (x = 0; x <= 7; x++) {
				if (light[x] > 0) light[x] -= max_step;
			}
			cur_spot += direction;
			if (cur_spot > 7) cur_spot = 0;
			if (cur_spot < 0) cur_spot = 7;
			light[cur_spot] = max_val;
			
			// Reset the cycle count
			cycle = 0;
			// Finished one more frame
			num--;
			// When we switch direction
			next_switch--;

			// Every so often switch the direction
			if (next_switch <= 0) {
				brake = 1;
				next_switch = 75 + (rand() % 150);
			}
			
			// If the breaks are on, start slowing down
			if (brake) {
				speed += 1;
				// If we're too slow, reverse direction, reset
				if (speed >= min_speed) {
					direction *= -1;
					brake = 0;
				}
			} else {
				// If the brakes aren't on and we aren't up to speed, speed up!
				if (speed > max_speed) speed -= 1;
			}
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

void line_off(int line)//send 'A' through 'H'
{	
	if (line < 6) PORTD |= (1<<(line+2));
	else PORTB |= (1<<(line-6));
        
}


void line_on(int line)//send 'A' through 'H'
{	
	if (line < 6) PORTD &= ~(1<<(line+2));
	else PORTB &= ~(1<<(line-6));
}
