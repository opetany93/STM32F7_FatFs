#ifndef __INIT_H__
#define __INIT_H__

// ======================================= INCLUDES ==============================================================
#include <stdint.h>
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

#include "AK_discovery_boards.h"
#include "AK_stm32_gpio.h"


// ===============================================================================================================
void init(void);
static void SystemClock_Config(void);
static void LedInit(void);
void ledOn(void);
void ledOff(void);
void ledToggle(void);

#endif // __INIT_H__
