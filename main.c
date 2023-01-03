/*
 * BME680.c
 *
 * Created: 12.12.2022 10:01:24
 * Author : Samuel Lüthi
 */ 
#define F_CPU 8000000

#include "Header.h"
#include <util/delay.h>
volatile unsigned char cRecieve[5];

int main(void)
{
	char cRET = 0;
	cRET = INIT();
	float TEMP;
	float PRESS;
	float HUM;
	float GAS;
	char Ziffern[10];
	char TEMPRATURE[] = {'T','E','M','P','R','A','T','U','R','E',':',' '};
	char PRESSURE[] =	{'P','R','E','S','S','U','R','E',':',' '};
	char HUMIDITY[] =	{'H','U','M','I','D','I','T','Y',':',' '};
	
	if (cRET)
	{
	//	return 1;
	}
	
    /* Replace with your application code */
    while (1) 
    {
		
		START_CONVERSION();
		TEMP = READ_TEMP_F();
		PRESS = READ_PRESS();
		HUM = READ_HUM ();
		//GAS = READ_GAS();
		
		for(int i = 0; i < 12; i++)
		{
			USART0SendByte(TEMPRATURE[i]);
		}	
		
		if (TEMP < 0)
		{
			USART0SendByte('-');
			TEMP = TEMP * -1.0;
		}
		
		
		
		Ziffern[0] = TEMP / 10;
		Ziffern[1] = TEMP  - Ziffern[0] * 10;
		Ziffern[2] = (TEMP - Ziffern[0] * 10 -  Ziffern[1]) * 10;
		Ziffern[3] = (TEMP - Ziffern[0] * 10 -  Ziffern[1] - ((double)Ziffern[2] / 10.0)) * 100;
		Ziffern[4] = (TEMP - Ziffern[0] * 10 -  Ziffern[1] - ((double)Ziffern[2] / 10.0) -  ((double)Ziffern[3] / 100.0)) * 1000;
		
		for (int i = 0; i < 2; i++)
		{
			USART0SendByte(Ziffern[i] + 0x30);
		}
		USART0SendByte(0x2E);
		for (int i = 0; i < 3; i++)
		{
			USART0SendByte(Ziffern[i+2] + 0x30);
		}
		
		USART0SendByte(',');
		USART0SendByte(' ');
		
		PRESS = PRESS / 100;
		Ziffern[0] = PRESS / 1000;
		Ziffern[1] = (PRESS - Ziffern[0] * 1000) / 100; 
		Ziffern[2] = (PRESS - Ziffern[0] * 1000 - Ziffern[1] * 100) / 10; 
		Ziffern[3] = (PRESS - Ziffern[0] * 1000 - Ziffern[1] * 100 - Ziffern[2] * 10); 
		for(int i = 0; i < 10; i++)
		{
			USART0SendByte(PRESSURE[i]);
		}
		for (int i = 0; i < 4; i++)
		{
			USART0SendByte(Ziffern[i] + 0x30);
		}
		
		USART0SendByte(',');
		USART0SendByte(' ');
		
		Ziffern[0] = HUM / 10;
		Ziffern[1] = HUM  - Ziffern[0] * 10;
		Ziffern[2] = (HUM - Ziffern[0] * 10 -  Ziffern[1]) * 10;
		Ziffern[3] = (HUM - Ziffern[0] * 10 -  Ziffern[1] - ((double)Ziffern[2] / 10.0)) * 100;
		for(int i = 0; i < 10; i++)
		{
			USART0SendByte(HUMIDITY[i]);
		}
		for (int i = 0; i < 2; i++)
		{
			USART0SendByte(Ziffern[i] + 0x30);
		}
		USART0SendByte(0x2E);
		for (int i = 0; i < 2; i++)
		{
			USART0SendByte(Ziffern[i+2] + 0x30);
		}
		
		USART0SendByte(0x0A);
		_delay_ms(500);
    }
}

char INIT ( void )
{
	char cRET = 0;
	USART0Init(103);

	
	cRET = twi_init(TWI_FREQ_100K_8M);
	cRET = INIT_BME();

	USART0SendByte(0x21);
	
	if (cRET)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

