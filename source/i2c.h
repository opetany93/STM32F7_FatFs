/*
 ===========================================================================================================================================================================
																																			Header FILE
																																			I2C DRIVER
																																			NUCLEO L476RG
																																			

  ===========================================================================================================================================================================
*/ 

#ifndef __i2c_H__
#define __i2c_H__

/* --------------------> INCLUDES */
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

/* --------------------> MACROS */
#define I2C_INIT_OK 		0x00;
#define I2C_INIT_ERROR 	0x01;

/* --------------------> FUNCTIONS <-------------------------------- */

uint8_t i2c_init(I2C_TypeDef* I2Cx);
uint8_t i2c1_is_device_ready(uint16_t dev_addres);
void read_mem(uint16_t dev_adr,uint16_t mem_adr,uint16_t mem_size, uint8_t *data, uint16_t size);
void write_mem(uint16_t dev_adr,uint16_t mem_adr,uint16_t mem_size, uint8_t *data, uint16_t size);

#endif // __i2c_H__
