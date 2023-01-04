/*
 * BME680.c
 *
 * Created: 03.01.2023 10:21:36
 *  Author: Samuel Lüthi
 */ 

#include "BME680.h"
volatile unsigned char cRecieve[5];

char INIT_BME ( void )
{
	unsigned char ucInitValues[2];
	unsigned char ucIdRegister = REG_ID;
	unsigned char cID = 0;
	
	twi_master_transmit(SLAVE_ADR_LOW, &ucIdRegister, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, &cID , 1);
	
	if (!cID)
	{
		return 1;
	}
	
	/*** WRITE RESET REGISTER ***/
	ucInitValues[0] = REG_RESET;
	ucInitValues[1] = VAL_RESET;
	twi_master_transmit(SLAVE_ADR_LOW, ucInitValues, 2, 0);
	
	/*** WRITE CTRL_HUM REGISTER ***/
	ucInitValues[0] = REG_CTRL_HUM;
	ucInitValues[1] = (0xBF & (0b00000111 & VAL_OSRS_1));
	twi_master_transmit(SLAVE_ADR_LOW, ucInitValues, 2, 0);
	
	/*** WRITE CONFIG REGISTER ***/
	ucInitValues[0] = REG_CONFIG;
	ucInitValues[1] = (0xFE & (0b00011100 & (VAL_FILTER_0 << 2)));
	twi_master_transmit(SLAVE_ADR_LOW, ucInitValues, 2, 0);
	
	/*** WRITE CTRL_MEAS REGISTER ***/
	ucInitValues[0] = REG_CTRL_MEAS;
	ucInitValues[1] = ((0b00000011 & VAL_MODE_FORCED) | (0b00011100 & (VAL_OSRS_1 << 2)) | (0b11100000 & (VAL_OSRS_1 << 5)));
	twi_master_transmit(SLAVE_ADR_LOW, ucInitValues, 2, 0);
	
	/*** WRITE CTRL_GAS REGISTER ***/
	ucInitValues[0] = REG_CTRL_GAS_0;
	ucInitValues[1] = (0b00001000 & (VAL_HEAT_OFF << 3));
	twi_master_transmit(SLAVE_ADR_LOW, ucInitValues, 2, 0);
	
	/*** WRITE CTRL_HUM REGISTER ***/
	ucInitValues[0] = REG_CTRL_GAS_1;
	ucInitValues[1] = (0b00010000 & (VAL_HEAT_ON << 4));
	twi_master_transmit(SLAVE_ADR_LOW, ucInitValues, 2, 0);
	
	return 0;
}

void START_CONVERSION ( void )
{
	unsigned char ucInitValues[2];
	
	ucInitValues[0] = REG_CTRL_MEAS;
	ucInitValues[1] = ((0b00000011 & VAL_MODE_FORCED) | (0b00011100 & (VAL_OSRS_1 << 2)) | (0b11100000 & (VAL_OSRS_1 << 5)));
	twi_master_transmit(SLAVE_ADR_LOW, ucInitValues, 2, 0);
	for(int i = 0; i < 50; i++)
	{
		asm("nop");
	}
}

float READ_TEMP_F ( void )
{
	int32_t t_fine = READ_TEMP_I();
	return t_fine / 5120.0;
}

int32_t READ_TEMP_I ( void )
{
	unsigned char cAdrWork = 0;
	uint32_t temp_adc = 0;
	uint16_t par_t1 = 0;
	int16_t par_t2 = 0;
	int8_t par_t3 = 0;
	double var1 = 0;
	double var2 = 0;
	int32_t t_fine = 0;
	
	cAdrWork = REG_TEMP_MSB;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 3);
	temp_adc = (0xFF000 & (((uint32_t)cRecieve[0]) << 12)) | (0x00FF0 & (cRecieve[1] << 4)) | (0x0000F & (cRecieve[2] >> 4));
	
	cAdrWork = 0xE9;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_t1 = (0xFF00 & (cRecieve[1] << 8)) | (0x00FF & (cRecieve[0]));
	
	cAdrWork = 0x8A;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_t2 = (0xFF00 & (cRecieve[1] << 8)) | (0x00FF & (cRecieve[0]));
	
	cAdrWork = 0x8C;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_t3 = (cRecieve[0]);
	
	var1 = (((double)temp_adc / 16384.0) - ((double)par_t1 / 1024.0)) * (double)par_t2;
	var2 = ((((double)temp_adc / 131072.0) - ((double)par_t1 / 8192.0)) * (((double)temp_adc / 131072.0) - ((double)par_t1 / 8192.0))) * ((double)par_t3 * 16.0);
	t_fine = var1 + var2;

	
	return t_fine;
}

float READ_PRESS ( void )
{
	int32_t t_fine = READ_TEMP_I();
	unsigned char cAdrWork = 0;
	double var1 = 0;
	double var2 = 0;
	double var3 = 0;
	double press_comp = 0;
	uint32_t press_adc = 0;
	uint16_t par_p1  = 0;
	int16_t par_p2  = 0;
	int8_t par_p3  = 0;
	int16_t par_p4  = 0;
	int16_t par_p5  = 0;
	int8_t par_p6  = 0;
	int8_t par_p7  = 0;
	int16_t par_p8  = 0;
	int16_t par_p9  = 0;
	int8_t par_p10  = 0;
	
	cAdrWork = REG_PRESS_MSB;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	press_adc = (0xFF000 & (((uint32_t)cRecieve[0]) << 12));
	
	cAdrWork = REG_PRESS_LSB;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	press_adc = press_adc | (0x00FF0 & (cRecieve[0] << 4)) | (0x0000F & (cRecieve[1] >> 4));
	
	cAdrWork = 0x8E;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_p1 = (0xFF00 & (((uint32_t)cRecieve[1]) << 8)) | (0x00FFF & (cRecieve[0]));
	
	cAdrWork = 0x90;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_p2 = (0xFF00 & (((uint32_t)cRecieve[1]) << 8)) | (0x00FFF & (cRecieve[0]));
	
	cAdrWork = 0x92;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	par_p3 = cRecieve[0];
	
	cAdrWork = 0x94;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_p4 = (0xFF00 & (((uint32_t)cRecieve[1]) << 8)) | (0x00FFF & (cRecieve[0]));
	
	cAdrWork = 0x96;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_p5 = (0xFF00 & (((uint32_t)cRecieve[1]) << 8)) | (0x00FFF & (cRecieve[0]));
	
	cAdrWork = 0x99;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	par_p6 = cRecieve[0];
	
	cAdrWork = 0x98;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_p7 = cRecieve[0];
	
	cAdrWork = 0x9C;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_p8 = (0xFF00 & (((uint32_t)cRecieve[1]) << 8)) | (0x00FFF & (cRecieve[0]));
	
	cAdrWork = 0x9E;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_p9 = (0xFF00 & (((uint32_t)cRecieve[1]) << 8)) | (0x00FFF & (cRecieve[0]));
	
	cAdrWork = 0xA0;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_p10 = cRecieve[0];
	
	
	var1 = ((double)t_fine / 2) - 64000.0;
	var2 = var1 * var1 * ((double)par_p6 / 131072);
	var2 = var2 + (var1 * (double)par_p5 * 2.0);
	var2 = (var2 / 4.0) + ((double)par_p4 * 65536.0);
	var1 = ((((double)par_p3 * var1 * var1) / 16384.0) + ((double)par_p2 * var1)) / 524288.0;
	var1 = (1.0 + (var1 / 32768.0)) * (double)par_p1;
	press_comp = 1048576.0 - (double)press_adc;
	press_comp = ((press_comp - (var2 / 4096.0)) * 6250.0) / var1;
	var1 = ((double)par_p9 * press_comp) / 2147483648.0;
	var2 = press_comp * ((double)par_p8 / 32768);
	var3 = (press_comp / 256.0) * (press_comp / 256.0) * (press_comp / 256.0) * (par_p10 / 131072.0);
	press_comp = press_comp + (var1 + var2 + var3 + ((double)par_p7 * 128.0)) / 16.0;
	
	return press_comp;
}

float READ_HUM ( void )
{
	double t_comp = READ_TEMP_F();
	unsigned char cAdrWork = 0;
	double var1 = 0;
	double var2 = 0;
	double var3 = 0;
	double var4 = 0;
	double var5 = 0;
	double var6 = 0;
	double hum_comp = 0;
	uint32_t hum_adc = 0;
	uint16_t par_h1  = 0;
	uint16_t par_h2  = 0;
	int8_t par_h3  = 0;
	int8_t par_h4  = 0;
	int8_t par_h5  = 0;
	int8_t par_h6  = 0;
	int8_t par_h7  = 0;
	
	cAdrWork = REG_HUM_MSB;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	hum_adc = (0xFF00 & (cRecieve[0] << 8)) | (0x00FF & (cRecieve[1]));
	
	
	cAdrWork = 0xE2;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_h1 = (0x0FF0 & (cRecieve[1] << 4)) | (0x000F & (cRecieve[0]));
	
	cAdrWork = 0xE1;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_h2 = (0x0FF0 & (cRecieve[0] << 4)) | (0x000F & ((cRecieve[1] & 0xF0) >> 4));
	
	cAdrWork = 0xE4;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	par_h3 = cRecieve[0];
	
	cAdrWork = 0xE5;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	par_h4 = cRecieve[0];
	
	cAdrWork = 0xE6;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	par_h5 = cRecieve[0];
	
	cAdrWork = 0xE7;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	par_h6 = cRecieve[0];
	
	cAdrWork = 0xE4;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	par_h7 = cRecieve[0];
	
	var1 = hum_adc - (((double)par_h1 * 16.0) + (((double)par_h3 / 2.0) * t_comp));
	var2 = var1 * (((double)par_h2 / 262144.0) * (1.0 + (((double)par_h4 / 16384.0) * t_comp) + (((double)par_h5 / 1048576.0) * t_comp * t_comp)));
	var3 = (double)par_h6 / 16384.0;

	var4 = (double)par_h7 / 2097152.0;
	hum_comp = var2 +((var3 + (var4 * t_comp)) * var2 * var2);
	
	
	return hum_comp;
}

float READ_GAS ( void )
{
	double t_comp = READ_TEMP_F();
	unsigned char cAdrWork = 0;
	unsigned char ucValWork[2];
	double var1 = 0;
	double gas_res = 0;
	double range_switching_error = 0;
	double gas_range = 0;
	uint32_t gas_adc = 0;
	int8_t par_g1  = 0;
	int16_t par_g2  = 0;
	int8_t par_g3  = 0;
	
	cAdrWork = 0xED;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_g1 = cRecieve[0];
	
	cAdrWork = 0xEB;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	par_g2 = (0xFF00 & (cRecieve[1] << 8)) | (0x00FF & (cRecieve[0] & 0xF0));
	
	cAdrWork = 0xEE;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	par_g3 = cRecieve[0];
	
	
	HEAT_GAS(par_g1, par_g2, par_g3);
	_delay_ms(100);
	
	cAdrWork = REG_GAS_R_MSB;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 2);
	gas_adc = (0xFF00 & (cRecieve[1] << 2)) | (0x00FF & (cRecieve[0] >> 6));
	//_delay_ms(200);
	
	
	cAdrWork = 0x2B;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	gas_range = (0x0F & (cRecieve[0]));
	
	cAdrWork = 0x04;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	range_switching_error = cRecieve[0];
	
	var1 = (1340.0 + 5.0 * range_switching_error) * gas_range;
	gas_res = var1 * gas_range / (gas_adc - 512.0 + var1);
	
	/*** WRITE CTRL_GAS_0 REGISTER ***/
	ucValWork[0] = REG_CTRL_GAS_1;
	ucValWork[1] = 0x00;
	twi_master_transmit(SLAVE_ADR_LOW, ucValWork, 2, 0);
	
	return gas_res;
}

void HEAT_GAS ( int8_t par_g1, int16_t par_g2, int8_t par_g3 )
{
	unsigned char cAdrWork = 0;
	unsigned char ucValWork[2];
	int32_t var1 = 0;
	int32_t var2 = 0;
	int32_t var3 = 0;
	int32_t var4 = 0;
	int32_t var5 = 0;
	int32_t res_heat_x100 = 0;
	uint8_t res_heat_x = 0;
	int8_t res_heat_range = 0;
	int8_t res_heat_value = 0;
	double ambient_temp = 0;
	ambient_temp = READ_TEMP_F();
	
	cAdrWork = 0x02;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	res_heat_range = (0x03 & (cRecieve[0] >> 4));
	
	cAdrWork = 0x00;
	twi_master_transmit(SLAVE_ADR_LOW, &cAdrWork, 1, 0);
	twi_master_receive(SLAVE_ADR_LOW, cRecieve, 1);
	res_heat_value = (0xFF & (cRecieve[0]));
	
	var1 = (((int32_t)ambient_temp * par_g3) / 10) << 8;
	var2 = (par_g1 + 784) * (((((par_g2 + 154009) * TARG_TEMP_GAS * 5) / 100) + 3276800) /10);
	var3 = var1 + (var2 >> 1);
	var4 = (var3 / (res_heat_range + 4));
	var5 = (131 * res_heat_value) + 65536;
	res_heat_x100 = (int32_t)(((var4 / var5) - 250) * 34);
	res_heat_x = (uint8_t)((res_heat_x100 + 50) / 100);
	
	ucValWork[0] = REG_GAS_WAIT_0;
	ucValWork[1] = 0x59;
	twi_master_transmit(SLAVE_ADR_LOW, ucValWork, 2, 0);
	
	/*** WRITE RES_WAIT REGISTER ***/
	ucValWork[0] = REG_RES_WAIT_0;
	ucValWork[1] = res_heat_x;
	twi_master_transmit(SLAVE_ADR_LOW, ucValWork, 2, 0);
	
	/*** WRITE CTRL_GAS_1 REGISTER ***/
	ucValWork[0] = REG_CTRL_GAS_0;
	ucValWork[1] = 0x00;
	twi_master_transmit(SLAVE_ADR_LOW, ucValWork, 2, 0);
	
	/*** WRITE CTRL_GAS_0 REGISTER ***/
	ucValWork[0] = REG_CTRL_GAS_1;
	ucValWork[1] = 0x10;
	twi_master_transmit(SLAVE_ADR_LOW, ucValWork, 2, 0);
	
	//_delay_ms(100);
	
	START_CONVERSION();
	
}