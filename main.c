/*
 * BME680.c
 *
 * Created: 12.12.2022 10:01:24
 * Author : Samuel Lüthi
 */ 

#include "Header.h"


int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

char INIT ( void )
{
	twi_init(TWI_FREQ_250K_8M);
	INIT_BME();
}

char INIT_BME ( void )
{
	char cInitValues[] = {}
	twi_master_transmit(SLAVE_ADR_LOW,)
}