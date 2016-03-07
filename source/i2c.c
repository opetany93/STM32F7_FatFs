/*
==========================================================================================================================
															SOURCE FILE
															I2C DRIVER
															MICHAL KRUK
=========================================================================================================================
*/ 
/* --------------------> INCLUDES */
#include "i2c.h"

/* -------------------->  HANDLE_STRUCTUREs */
	I2C_HandleTypeDef I2C_HandleStruct;
	
/* --------------------> VARIABLES */
volatile uint8_t recive_stat;

volatile uint8_t comp_buf_X[2];
volatile uint8_t comp_buf_1[8];

/*
 ===========================================================================================================================================================================
																																			
																																			Inicjalizacja I2C1

  ===========================================================================================================================================================================
*/ 

uint8_t i2c_init(I2C_TypeDef* I2Cx)
{
	//zmienna do zwracania statusu funkcji
	uint8_t i2c_status;
	
	//inicjalizacja struktur od GPIO
	GPIO_InitTypeDef     GPIO_InitStruct;
	
	i2c_status = I2C_INIT_ERROR;
	
		//Enable Clock for GPIOB
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		//Enable Clock for I2C 1
		if(I2Cx == I2C1) __HAL_RCC_I2C1_CLK_ENABLE();		//If I2C1
		if(I2Cx == I2C2) __HAL_RCC_I2C2_CLK_ENABLE();		//If I2C2
		if(I2Cx == I2C3) __HAL_RCC_I2C3_CLK_ENABLE();		//If I2C3
	
		//GPIO Configuration 
		
		/* Configure GPIO as I2C1_SDA */
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
			
		/* Configure GPIO as I2C1_SCL */
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
				
		//I2C Configuration, konfigurqacja rejestru timing, f I2C , rise and fall time 
		I2C_HandleStruct.Instance = I2Cx;
		I2C_HandleStruct.Init.Timing =	0x00702991; 												//100kHz 0x30A54E69
		I2C_HandleStruct.Init.OwnAddress1 = 0x0;														//Own Address potrzebne sa do pracy w trybie slave mode.
		I2C_HandleStruct.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		I2C_HandleStruct.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
		I2C_HandleStruct.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
		I2C_HandleStruct.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
		I2C_HandleStruct.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
				
		i2c_status = HAL_I2C_Init(&I2C_HandleStruct);
							
		//Configure Analogue filter (Filtry szumowe musza byc zawsze skonfigurowane przed wlaczniem I2C
		HAL_I2CEx_AnalogFilter_Config(&I2C_HandleStruct, I2C_ANALOGFILTER_ENABLED);
				
				
		__HAL_I2C_ENABLE(&I2C_HandleStruct);	
				
		/* Peripheral interrupt init*/
//	HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
//	HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);

		return i2c_status;
}

/*
 ===========================================================================================================================================================================
																																			
																																			Test I2C1

  ===========================================================================================================================================================================
*/ 
uint8_t i2c1_is_device_ready(uint16_t dev_addres)
{
	// deklaracja zwracanego parametru
	uint8_t i2c_device;

	i2c_device = HAL_I2C_IsDeviceReady(&I2C_HandleStruct, dev_addres , 10,100);
	

	return i2c_device;	
}

/*
 ===========================================================================================================================================================================
																																			
																																				I2C1 RECIVE

  ===========================================================================================================================================================================
*/ 
void read_mem(uint16_t dev_adr,uint16_t mem_adr,uint16_t mem_size, uint8_t *data, uint16_t size)
{
		/* Czwarty parametr to rozmiar odczytywanego obszaru pamieci, przewaznie jest to 1 bajt
			6 parametr to ilosc bitów które chcemy po kolei odczytac.
		*/
		HAL_I2C_Mem_Read(&I2C_HandleStruct, dev_adr, mem_adr, mem_size,data, size,100);
}
/*
 ===========================================================================================================================================================================
																																			
																																				I2C1 RECIVE

  ===========================================================================================================================================================================
*/ 
void write_mem(uint16_t dev_adr,uint16_t mem_adr,uint16_t mem_size, uint8_t *data, uint16_t size)
{
		HAL_I2C_Mem_Write(&I2C_HandleStruct, dev_adr, mem_adr, mem_size,data, size,100);
}
/*
 ===========================================================================================================================================================================
																																			
																																		Koniec Pliku

  ===========================================================================================================================================================================
*/ 
