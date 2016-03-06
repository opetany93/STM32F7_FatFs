#ifndef _AK_STM32_FATFS_H__
#define _AK_STM32_FATFS_H__

#include <stdint.h>
#include "ff.h"
#include "diskio.h"

 /* par SDCARD pinouts
 *
 * Library works with SPI or SDIO mode. Also, when in SDIO mode, you can set to 1- or 4-bit mode.
 * By default, SDIO with 4-bit mode is used, so you will look into right column on table below.
 * 
 * SD CARD PINS	   
   _________________
  / 1 2 3 4 5 6 7 8 |  NR   |SDIO INTERFACE                               |SPI INTERFACE
 /                  |       |NAME     STM32Fxxx     DESCRIPTION           |NAME   STM32Fxxx   DESCRIPTION 
/ 9                 |       |         4-BIT  1-BIT                        | 
|                   |       |                                             | 
|                   |   1   |CD/DAT3  PC11   -      Connector data line 3 |CS     PB5         Chip select for SPI 
|                   |   2   |CMD      PD2    PD2    Command/Response line |MOSI   PA7         Data input for SPI 
|                   |   3   |VSS1     GND    GND    GND                   |VSS1   GND         GND 
|   SD CARD Pinout  |   4   |VDD      3.3V   3.3V   3.3V Power supply     |VDD    3.3V        3.3V Power supply 
|                   |   5   |CLK      PC12   PC12   Clock                 |SCK    PA5         Clock for SPI 
|                   |   6   |VSS2     GND    GND    GND                   |VSS2   GND         GND 
|                   |   7   |DAT0     PC8    PC8    Connector data line 0 |MISO   PA6         Data output for SPI 
|                   |   8   |DAT1     PC9    -      Connector data line 1 |-      -           - 
|___________________|   9   |DAT2     PC10   -      Connector data line 2 |-      -           - 

 * By default, SDIO with 4-bit communication is used.
 */

/**
 * @brief  FATFS size structure
 */
typedef struct {
	uint32_t Total; /*!< Total size of memory */
	uint32_t Free;  /*!< Free size of memory */
} AK_FATFS_Size_t;

FRESULT AK_FATFS_GetDriveSize(char* str, AK_FATFS_Size_t* SizeStruct);

#endif // _AK_STM32_FATFS_H__
