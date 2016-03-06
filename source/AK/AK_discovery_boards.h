#ifndef __DISCOVERY_BOARDS_H__
#define __DISCOVERY_BOARDS_H__

#ifdef STM32F4_DISCOVERY

#define LED_PORT		GPIOD
#define LED1				GPIO_PIN_12
#define LED2				GPIO_PIN_13
#define LED3				GPIO_PIN_14
#define LED4				GPIO_PIN_15
#define LED_ALL			GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15

#endif

#ifdef STM32F7_DISCOVERY

#define LED_PORT		GPIOI
#define LED1				GPIO_PIN_1
#define LED_ALL			LED1

#endif


#endif //__DISCOVERY_BOARDS_H__
