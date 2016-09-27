#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>

void button();
void matrix();
void led();
uint8_t state = 0;
uint8_t led_bit = 0x01;
uint8_t m_button = 0;
uint8_t m_offset = 0;

uint8_t row = 0x08;

uint8_t sev_seg[4] = {11,11,11,11};
uint8_t digit = 0;
char seg = 3;

int main(void)
{
    DDRB = 0xF0;
    DDRD = 0xF0;
    DDRC = 0x78;
    DDRA = 0xFF;

    ///TIMER0 inicializálás
    TCCR0 = (1<<CS02)| (1<<CS00);   //időosztás
    TIMSK = (1<<TOIE0);             //timer0 engedélyezés
    sei();                          //globális interrupt engedélyezés

    while(1)
    {
        matrix();
        if(m_button != 12){
            switch(m_button){
                case 11:m_offset = 0; break;
                default:if(m_offset < 4){sev_seg[m_offset] = m_button; m_offset++;}
            }
        }

        /*led();
        button();*/
    }

    return 0;
}


ISR(TIMER0_OVF_vect) {

    digit = (digit+1)%4;
    seg = seg-1;
    if(seg < 0) seg = 3;
    PORTA = 0x80 + (digit<<4) + sev_seg[(seg + m_offset)%4];

}


void led()
{
    _delay_ms(500);
    _delay_ms(500);
    PORTB = led_bit<<4;
    PORTD = led_bit;

    if(state == 0) led_bit<<=1;
    else led_bit>>=1;

    if(led_bit == 0x80)state = 1;
    if(led_bit == 0x01)state = 0;
}

void button()
{

    switch (PING)
    {
        case 1:{
            if((led_bit <0x80) && (state == 0))state=1;    ///G0
            while(PING);
            break;
        }
        case 2:{
            if((led_bit >0x01) && (state == 1))state=0;    ///G1
            while(PING);
            break;
        }
    }
}

void matrix()
{
const unsigned char billtomb[12] = { 69 , 14, 13, 11, 22, 21, 19, 38, 37, 35, 70, 67 };
unsigned char num, bill;

    	PORTC = row;            ///címzés
		_delay_ms( 5 );         ///várakozás a tranyóra
    	bill  = PINC & 0x7f;    ///maszkolás 0b01111111
   	 	num = 0;                ///tmb index törlése

        while( num<12 )
        {
       		if( bill == billtomb[num] ){    ///tömb elemek ellenõrzése
                m_button = num;             ///ha egyezés van kiküldjük a számot
				while(PINC == billtomb[num] );  ///prellmentesítés
	  			break;                      ///folyamat befejezése
            }
        	else{
                m_button = 12;
                num++;
            }
        }
        if( row == 0x40 )row = 0x08;    ///sorcímzés léptetése
        else row = row << 1;            ///
}

void init()
{

    DDRG=0x00;
    DDRC = 0x78;
    PORTG=0x00;
    DDRB=0xF0;
    DDRD=0xF0;
    PORTB=0x00;
    PORTD=0;
    DDRA=0xFF;
}
