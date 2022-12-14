/*
 * AVR TWI Library
 */

#include "avr-twi-lib.h"

// Send start condition
static ret_code_t twi_send_start(void)
{
	// Send start condition
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

	// Wait for operation to finish
	while(!(TWCR & (1 << TWINT)));

	// Check TWSR Status register
	if(TWI_STATUS != 0x08)
	{
		return TWI_ERR_START;
	}
	return TWI_SUCCESS;
}

// Transmit slave address with write flag
static ret_code_t twi_send_sla_w(uint8_t slave_addr)
{
	// Write slave address
	TWDR = TWI_SLA_W(slave_addr);
	// Write TWINT bit
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait for operation to finish
	while(!(TWCR & (1 << TWINT)));

	if(TWI_STATUS != 0x18)
	{
		if(TWI_STATUS == 0x20)
		{
			// Received NACK
			return TWI_ERR_NACK;
		}
		// unknow error code in TWSR
		return TWI_ERROR;
	}
	return TWI_SUCCESS;
}

// Transmit slave address with read flag
static ret_code_t twi_send_sla_r(uint8_t slave_addr)
{
	// Write slave address
	TWDR = TWI_SLA_R(slave_addr);
	// Write TWINT bit
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait for operation to finish
	while(!(TWCR & (1 << TWINT)));

	if(TWI_STATUS != 0x40)
	{
		if(TWI_STATUS == 0x48)
		{
			// Received NACK
			return TWI_ERR_NACK;
		}
		// unknow error code in TWSR
		return TWI_ERROR;
	}
	return TWI_SUCCESS;
}

// Send data byte
static ret_code_t twi_send_data(uint8_t data)
{
	// Write data byte
	TWDR = data;
	// Write TWINT bit
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait for operation to finish
	while(!(TWCR & (1 << TWINT)));

	if(TWI_STATUS != 0x28)
	{
		if(TWI_STATUS == 0x30)
		{
			// Received NACK
			return TWI_ERR_NACK;
		}
		// unknow error code in TWSR
		return TWI_ERROR;
	}

	return TWI_SUCCESS;
}

// Read data byte
static uint8_t twi_read_data(char read_ack)
{
	if(read_ack)
	{
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
		while(!(TWCR & (1 << TWINT)));
		if(TWI_STATUS != 0x50)
		{
			return TWI_STATUS;
		}
	}
	else
	{
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT)));
		if(TWI_STATUS != 0x58)
		{
			return TWI_STATUS;
		}
	}
	uint8_t data = TWDR;

	return data;
}

// Send stop condition
ret_code_t twi_send_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

	return TWI_SUCCESS;
}

// Initialise TWI interface
ret_code_t twi_init(twi_freq_t twi_freq)
{
	// Set power reduction register
#if defined (__AVR_ATmega2561__)
	PRR0 &= ~(1 << PRTWI);
#elif defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
	PRR &= ~(1 << PRTWI);
#endif

	// Set frequency
	switch(twi_freq)
	{
		case TWI_FREQ_100K_8M:
			// Set TWBR 32 and Prescaler 1 -> 8MHz / (16 + 2 * 32 * 1) = 100kHz
			TWBR = 32;
			break;
		case TWI_FREQ_100K_16M:
			// Set TWBR 32 and Prescaler 1 -> 16MHz / (16 + 2 * 72 * 1) = 100kHz
			TWBR = 72;
			break;
		case TWI_FREQ_100K_20M:
			// Set TWBR 32 and Prescaler 1 -> 20MHz / (16 + 2 * 92 * 1) = 100kHz
			TWBR = 92;
			break;
		case TWI_FREQ_250K_8M:
			// Set TWBR 8 and Prescaler 1 -> 8MHz / (16 + 2 * 8 * 1) = 250kHz
			TWBR = 8;
			break;
		case TWI_FREQ_250K_16M:
			// Set TWBR 32 and Prescaler 1 -> 16MHz / (16 + 2 * 24 * 1) = 100kHz
			TWBR = 24;
			break;
		case TWI_FREQ_250K_20M:
			// Set TWBR 32 and Prescaler 1 -> 20MHz / (16 + 2 * 32 * 1) = 100kHz
			TWBR = 32;
			break;
		case TWI_FREQ_400K_8M:
			// Set TWBR 2 and Prescaler 1 -> 8MHz / (16 + 2 * 2 * 1) = 400kHz
			TWBR = 2;
			break;
		case TWI_FREQ_400K_16M:
			// Set TWBR 32 and Prescaler 1 -> 16MHz / (16 + 2 * 12 * 1) = 100kHz
			TWBR = 12;
			break;
		case TWI_FREQ_400K_20M:
			// Set TWBR 32 and Prescaler 1 -> 20MHz / (16 + 2 * 17 * 1) = 100kHz
			TWBR = 17;
			break;
		case TWI_FREQ_1M_16M:
			// Set TWBR 2 and Prescaler 1 -> 16MHz / (16 + 2 * 0 * 1) = 1MHz
			TWBR = 0;
			break;
		case TWI_FREQ_1M_20M:
			// Set TWBR 32 and Prescaler 1 -> 20MHz / (16 + 2 * 2 * 1) = 100kHz
			TWBR = 2;
			break;
		default:
			return TWI_ERR_FREQ;
			break;
	}
	return TWI_SUCCESS;
}

// Transmit on the TWI Interface as a master
ret_code_t twi_master_transmit(uint8_t slave_addr, uint8_t* pData, uint8_t len, char repeat_start)
{
	// Send start condition
	ret_code_t error_code = twi_send_start();
	if(error_code != TWI_SUCCESS)
	{
		return error_code;
	}

	// Send slave address with write flag
	error_code = twi_send_sla_w(slave_addr);
	if(error_code != TWI_SUCCESS)
	{
		return error_code;
	}

	// Send data byte or bytes
	for(int i = 0; i < len; i++)
	{
		error_code = twi_send_data(pData[i]);
		if(error_code != TWI_SUCCESS)
		{
			return error_code;
		}
	}

	// Send stop condition
	if(!repeat_start)
	{
		twi_send_stop();
	}

	return TWI_SUCCESS;
}

ret_code_t twi_master_receive(uint8_t slave_addr, uint8_t* pData, uint8_t len)
{
	// Send start condition
	ret_code_t error_code = twi_send_start();
	if(error_code != TWI_SUCCESS)
	{
		return error_code;
	}

	// Send slave address with write flag
	error_code = twi_send_sla_r(slave_addr);
	if(error_code != TWI_SUCCESS)
	{
		return error_code;
	}

	/* Read single or multiple data byte and send ack */
	for(int i = 0; i < len-1; i++)
	{
		pData[i] = twi_read_data(TWI_READ_ACK);
	}
	pData[len-1] = twi_read_data(TWI_READ_NACK);

	/* Send STOP condition */
	twi_send_stop();

	return TWI_SUCCESS;
}
