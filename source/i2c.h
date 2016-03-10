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
#include "stm32F7xx.h"
#include "stm32F7xx_hal.h"

/* --------------------> MACROS */
#define I2C_INIT_OK 		0x00
#define I2C_INIT_ERROR 	0x01



/* --------------------> FUNCTIONS */

/* --------------------> Funkcjie do podmieniania (w zaleznosci od uzywanego proc*/
void hal_rcc_gpio_conf_i2c1(void);
void hal_rcc_gpio_conf_i2c2(void);
void hal_rcc_gpio_conf_i2c3(void);

uint8_t i2c_init(I2C_TypeDef* I2Cx,uint32_t TIMING, GPIO_TypeDef  *GPIO_SDAx, uint32_t GPIO_SDA_PIN, GPIO_TypeDef  *GPIO_SCLx, 
			uint32_t GPIO_SCL_PIN, uint8_t GPIO_AF);
			
void gpio_i2c_config(GPIO_TypeDef  *GPIO_SDAx, uint32_t GPIO_SDA_PIN, GPIO_TypeDef  *GPIO_SCLx, 
uint32_t GPIO_SCL_PIN, uint8_t GPIO_AF);

void i2c_set_irq(IRQn_Type I2C_IRQ_TYPE,uint32_t IRQ_SOURCE, uint32_t PreemptPriority, uint32_t SubPriority );		
			
uint8_t i2c1_is_device_ready(uint16_t dev_addres);
void read_mem(uint16_t dev_adr,uint16_t mem_adr,uint16_t mem_size, uint8_t *data, uint16_t size);
void write_mem(uint16_t dev_adr,uint16_t mem_adr,uint16_t mem_size, uint8_t *data, uint16_t size);




#endif // __i2c_H__
