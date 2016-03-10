/*
 ===========================================================================================================================================================================
																																			SOURCE FILE
																																			MPU-6050 DRIVER
																																			NUCLEO L476RG
																																			MICHAL KRUK

  ===========================================================================================================================================================================
*/ 


/* --------------------> INCLUDES */
#include "i2c.h"

/* -------------------->  HANDLE_STRUCTUREs */
	I2C_HandleTypeDef I2C_HandleStruct;
/* --------------------> VARIABLES */
volatile uint8_t recive_stat;


volatile uint8_t comp_buf_X[2];
volatile uint8_t comp_buf_1[8];
/* --------------------> FUNCTIONS */
/*
 ===========================================================================================================================================================================
																																			
																																			Funkcje do ewentualnego podmieniania ( w zaleznosci 
																																			uzywanego uC)

  ===========================================================================================================================================================================
*/ 
/* -----------------------------> Funkce uruchamiajace zegary dla portu GPIO i danego I2c */

void hal_rcc_gpio_conf_i2c1(void){
			//Enable Clock for GPIOB
			__HAL_RCC_GPIOB_CLK_ENABLE();
			__HAL_RCC_I2C1_CLK_ENABLE();		
}

void hal_rcc_gpio_conf_i2c2(void){
			//Enable Clock for GPIOB
			__HAL_RCC_GPIOB_CLK_ENABLE();
			__HAL_RCC_I2C2_CLK_ENABLE();		
}

void hal_rcc_gpio_conf_i2c3(void){
			//Enable Clock for GPIOB
			__HAL_RCC_GPIOC_CLK_ENABLE();
			__HAL_RCC_I2C3_CLK_ENABLE();		
}

/* ----------------------------------------> Konfiguracja GPIO 
 * Mozliwe ze na halu do niektórych procków z seri F trza bedzie to nieco zmodyfikowac dlatego jest na górze tego 
 * pliku!
*/

void gpio_i2c_config(GPIO_TypeDef  *GPIO_SDAx, uint32_t GPIO_SDA_PIN, GPIO_TypeDef  *GPIO_SCLx, 
			uint32_t GPIO_SCL_PIN, uint8_t GPIO_AF){
			
	//inicjalizacja struktur od GPIO
	GPIO_InitTypeDef     GPIO_InitStruct;
			
	//GPIO Configuration 
	gpio_i2c_config(GPIO_SDAx, GPIO_SDA_PIN, GPIO_SCLx, GPIO_SCL_PIN,GPIO_AF);
	/* Configure GPIO as I2C1_SDA */
	GPIO_InitStruct.Pin = GPIO_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = GPIO_AF;
	HAL_GPIO_Init(GPIO_SDAx, &GPIO_InitStruct);

	/* Configure GPIO as I2C1_SCL */
	GPIO_InitStruct.Pin = GPIO_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = GPIO_AF;
	HAL_GPIO_Init(GPIO_SCLx, &GPIO_InitStruct);	
}
/*
 ===========================================================================================================================================================================
																																			
																																			Inicjalizacja I2C1

  ===========================================================================================================================================================================
*/ 

/* OPIS parametrów
	--I2Cx: Nazwa inicjalizowanego I2C np. I2C1
	--TIMING: Wartosc która wylicza Cube, zale|y od predkosci I2C ktora chesz ustawic
	--GPIO_SDAx: Podaj na którym porcie znajduje sie pin SDA danego I2C  np. GPIOB
	--GPIO_SDA_PIN: Podaj pin na którym jest SDA danego I2C np. GPIO_PIN_6
	--GPIO_SCLx: tak jak GPIO_SDAx
	--GPIO_SCL_PIN tak jak GPO_SDA_PIN
	--GPIO_AF podaj AF GPIO np GPIO_AF4_I2C1
*/

uint8_t i2c_init(I2C_TypeDef* I2Cx,uint32_t TIMING, GPIO_TypeDef  *GPIO_SDAx, uint32_t GPIO_SDA_PIN, GPIO_TypeDef  *GPIO_SCLx, 
			uint32_t GPIO_SCL_PIN, uint8_t GPIO_AF){
	
	//zmienna do zwracania statusu funkcji
	uint8_t i2c_status;
		
	i2c_status = I2C_INIT_ERROR;
		
	if(I2Cx == I2C1) hal_rcc_gpio_conf_i2c1();
	if(I2Cx == I2C2) hal_rcc_gpio_conf_i2c2();
	if(I2Cx == I2C3) hal_rcc_gpio_conf_i2c3();				

	//GPIO init
	gpio_i2c_config( GPIO_SDAx , GPIO_SDA_PIN , GPIO_SCLx , GPIO_SCL_PIN , GPIO_AF);
				
	//I2C Configuration			
	I2C_HandleStruct.Instance = I2Cx;
	//konfigurqacja rejestru timing, wartosc timing uzyskuje sie z cuba
	I2C_HandleStruct.Init.Timing =	TIMING; 
	I2C_HandleStruct.Init.OwnAddress1 = 0x0;	//Own Address potrzebne sa do pracy w trybie slave mode.
	I2C_HandleStruct.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2C_HandleStruct.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	I2C_HandleStruct.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	I2C_HandleStruct.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	I2C_HandleStruct.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	
	i2c_status = HAL_I2C_Init(&I2C_HandleStruct);
				
	//Configure Analogue filter (Filtry szumowe musza byc zawsze skonfigurowane przed wlaczniem I2C
	HAL_I2CEx_AnalogFilter_Config(&I2C_HandleStruct, I2C_ANALOGFILTER_ENABLED);
	
	__HAL_I2C_ENABLE(&I2C_HandleStruct);	

				
		return i2c_status;
}
/*
 ===========================================================================================================================================================================
																																			
																																			Inicjalizacja przerwan od I2C

  ===========================================================================================================================================================================
*/ 
/*
I2C_IRQ_TYPE:  External interrupt number. np. : I2C1_EV_IRQn, I2C1_ER_IRQn
IRQ_SOURCE:	
  *        This parameter can be one of the following values:
  *            @arg I2C_IT_ERRI: Errors interrupt enable
  *            @arg I2C_IT_TCI: Transfer complete interrupt enable
  *            @arg I2C_IT_STOPI: STOP detection interrupt enable
  *            @arg I2C_IT_NACKI: NACK received interrupt enable
  *            @arg I2C_IT_ADDRI: Address match interrupt enable
  *            @arg I2C_IT_RXI: RX interrupt enable
  *            @arg I2C_IT_TXI: TX interrupt enable

*/
void i2c_set_irq(IRQn_Type I2C_IRQ_TYPE,uint32_t IRQ_SOURCE, uint32_t PreemptPriority, uint32_t SubPriority ){
	HAL_NVIC_SetPriority(I2C_IRQ_TYPE, PreemptPriority, SubPriority);
	HAL_NVIC_EnableIRQ(I2C_IRQ_TYPE);
	
	__HAL_I2C_ENABLE_IT(&I2C_HandleStruct,IRQ_SOURCE);
}

/*
 ===========================================================================================================================================================================
																																			
																																			Test I2C1

  ===========================================================================================================================================================================
*/ 

uint8_t i2c1_is_device_ready(uint16_t dev_addres){

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
void read_mem(uint16_t dev_adr,uint16_t mem_adr,uint16_t mem_size, uint8_t *data, uint16_t size){
	
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
void write_mem(uint16_t dev_adr,uint16_t mem_adr,uint16_t mem_size, uint8_t *data, uint16_t size){

		
		HAL_I2C_Mem_Write(&I2C_HandleStruct, dev_adr, mem_adr, mem_size,data, size,100);

}
/*
 ===========================================================================================================================================================================
																																			
																																		Koniec Pliku

  ===========================================================================================================================================================================
*/ 
