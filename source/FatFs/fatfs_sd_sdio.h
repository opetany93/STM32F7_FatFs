#ifndef __fatfs_sd_sdio_H__
#define __fatfs_sd_sdio_H__

#include "stm32f7xx_hal.h"
#include "diskio.h"

#define SD_CardInfo HAL_SD_CardInfoTypedef

#define MSD_OK                        		((uint8_t)0x00)
#define MSD_ERROR                     		((uint8_t)0x01)
#define MSD_ERROR_SD_NOT_PRESENT      		((uint8_t)0x02)

#define SD_PRESENT               					((uint8_t)0x01)
#define SD_NOT_PRESENT           					((uint8_t)0x00)

#define SD_DATATIMEOUT           					((uint32_t)100000000)

//=========================================================================================
/* DMA definitions for SD DMA transfer */
#define __DMAx_TxRx_CLK_ENABLE            __HAL_RCC_DMA2_CLK_ENABLE
#define SD_DMAx_Tx_CHANNEL                DMA_CHANNEL_4
#define SD_DMAx_Rx_CHANNEL                DMA_CHANNEL_4
#define SD_DMAx_Tx_STREAM                 DMA2_Stream6  
#define SD_DMAx_Rx_STREAM                 DMA2_Stream3  
#define SD_DMAx_Tx_IRQn                   DMA2_Stream6_IRQn
#define SD_DMAx_Rx_IRQn                   DMA2_Stream3_IRQn
#define SD_DMAx_Tx_IRQHandler             DMA2_Stream6_IRQHandler   
#define SD_DMAx_Rx_IRQHandler             DMA2_Stream3_IRQHandler

//=========================================================================================
/* Set default value for CD pins */
#define SD_DETECT_GPIO_PORT             	GPIOC
#define SD_DETECT_PIN             				GPIO_PIN_13
#define SD_DETECT_GPIO_CLK_ENABLE()    		__HAL_RCC_GPIOC_CLK_ENABLE()

//=========================================================================================
/* Drivers function declarations */
DSTATUS FATFS_SD_SDIO_disk_initialize(void);
DSTATUS FATFS_SD_SDIO_disk_status(void);
DRESULT FATFS_SD_SDIO_disk_ioctl(BYTE cmd, void *buff);
DRESULT FATFS_SD_SDIO_disk_read(BYTE *buff, DWORD sector, UINT count);
DRESULT FATFS_SD_SDIO_disk_write(const BYTE *buff, DWORD sector, UINT count);

//=========================================================================================
/* Function declarations */
static uint8_t 		SD_Init(void);
uint8_t 					SD_DeInit(void);
static void 			SD_MspInit(void);
static void 			SD_Detect_MspInit(void);
static uint8_t 		SD_IsDetected(void);

uint8_t 	SD_ReadBlocks_DMA(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks);
uint8_t 	SD_WriteBlocks_DMA(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks);

uint8_t 	SD_ReadBlocks(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks);
uint8_t 	SD_WriteBlocks(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks);
uint8_t 	SD_Erase(uint64_t StartAddr, uint64_t EndAddr);
HAL_SD_TransferStateTypedef SD_GetStatus(void);
void    	SD_GetCardInfo(HAL_SD_CardInfoTypedef *CardInfo);


#endif // __fatfs_sd_sdio_H__
