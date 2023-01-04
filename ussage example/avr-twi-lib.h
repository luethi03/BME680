/*
 * AVR TWI Library
 */

#ifndef _AVR_TWI_LIB_H_
#define _AVR_TWI_LIB_H_

#include <avr/io.h>

#define TWI_STATUS			(TWSR & 0xF8)
#define TWI_WRITE				0
#define TWI_READ				1
#define TWI_SLA_W(ADDR)	((ADDR << 1) | TWI_WRITE)
#define TWI_SLA_R(ADDR)	((ADDR << 1) | TWI_READ)
#define TWI_READ_ACK		1
#define TWI_READ_NACK		0

// Error codes
#define TWI_SUCCESS			0
#define TWI_ERROR			1
#define TWI_ERR_FREQ		2
#define TWI_ERR_START		3
#define TWI_ERR_NACK		4



typedef uint16_t ret_code_t;

typedef enum{
	TWI_FREQ_100K_8M,		//  8MHz CPU Clock
	TWI_FREQ_100K_16M,		// 16MHz CPU Clock
	TWI_FREQ_100K_20M,		// 20MHz CPU Clock
	TWI_FREQ_250K_8M,		//  8MHz CPU Clock
	TWI_FREQ_250K_16M,		// 16MHz CPU Clock
	TWI_FREQ_250K_20M,		// 20MHz CPU Clock
	TWI_FREQ_400K_8M,		//  8MHz CPU Clock
	TWI_FREQ_400K_16M,		// 16MHz CPU Clock
	TWI_FREQ_400K_20M,		// 20MHz CPU Clock
	TWI_FREQ_1M_16M,		// 16MHz CPU Clock
	TWI_FREQ_1M_20M			// 20MHz CPU Clock
} twi_freq_t;

/**
 * @brief		twi_init
 * 				Init the 2-wire interface
 *
 * @param		(twi_freq_t)	twi_freq - SCL frequency
 *
 * @return	(ret_code_t)	Error code
 */
ret_code_t twi_init(twi_freq_t twi_freq);

/**
 * @brief		twi_master_transmit
 * 					Transmit data on the TWI bus as a master
 *
 * @param		(uint8_t)			slave_addr - Slave address
 * @param		(uint8_t*)			pData - Data to be transmitted
 * @param		(uint8_t)			len - Length of data in bytes
 * @param		(uint8_t)			repeat_start - Will send a stop signal if 0
 *
 * @return	(ret_code_t)	Error code
 */
ret_code_t twi_master_transmit(uint8_t slave_addr, uint8_t* pData, uint8_t len, char repeat_start);

/**
 * @brief		twi_master_receive
 * 					Receive data on the TWI bus as a master
 *
 * @param		(uint8_t)			slave_addr - Slave address
 * @param		(uint8_t*)			pData - Received data will be stored here
 * @param		(uint8_t)			len - Length of data in bytes
 *
 * @return	(ret_code_t)	Error code
 */

ret_code_t twi_master_receive(uint8_t slave_addr, uint8_t* pData, uint8_t len);

#endif
