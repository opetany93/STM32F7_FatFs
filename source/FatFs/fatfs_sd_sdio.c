#include "fatfs_sd_sdio.h"
#include "AK_stm32_gpio.h"

/* Block Size in Bytes */
#define BLOCK_SIZE                512

/* Status of SDCARD */
static volatile DSTATUS Stat = STA_NOINIT;

/***************************************************************/
/*                  		SDCARD DETECT                      		*/
/**************************************************************/
/* SDCARD detect function */
static uint8_t SD_IsDetected(void) 
{
  return !( AK_GPIO_GetInputPinValue(SD_DETECT_GPIO_PORT, SD_DETECT_PIN) );
}


/**************************************************************/
/*                    LOW LEVEL FUNCTIONS                     */
/**************************************************************/

DSTATUS FATFS_SD_SDIO_disk_initialize(void)
{
	Stat = STA_NOINIT;
	
	/* Configure the SDCARD device */
	if (SD_Init() == MSD_OK) Stat &= ~STA_NOINIT;
	else Stat |= STA_NOINIT;

	return Stat;
}

//=========================================================================================
DSTATUS FATFS_SD_SDIO_disk_status(void) 
{
	Stat = STA_NOINIT;

	/* Check SDCARD status */
	if (SD_GetStatus() == MSD_OK) Stat &= ~STA_NOINIT;
	else Stat |= STA_NOINIT;

	return Stat;
}

//=========================================================================================
DRESULT FATFS_SD_SDIO_disk_ioctl(BYTE cmd, void *buff) 
{
	DRESULT res = RES_ERROR;
	HAL_SD_CardInfoTypedef CardInfo;
  
	/* Check if init OK */
	if (Stat & STA_NOINIT) return RES_NOTRDY;
  
	switch (cmd) 
	{
		/* Make sure that no pending write process */
		case CTRL_SYNC :
			res = RES_OK;
			break;

		/* Get number of sectors on the disk (DWORD) */
		case GET_SECTOR_COUNT :
			SD_GetCardInfo(&CardInfo);
			*(DWORD *)buff = CardInfo.CardCapacity / BLOCK_SIZE;
			res = RES_OK;
			break;

		/* Get R/W sector size (WORD) */
		case GET_SECTOR_SIZE :
			*(WORD *)buff = BLOCK_SIZE;
			res = RES_OK;
			break;

		/* Get erase block size in unit of sector (DWORD) */
		case GET_BLOCK_SIZE :
			*(DWORD*)buff = BLOCK_SIZE;
			break;

		default:
			res = RES_PARERR;
	}
  
	return res;
}

//=========================================================================================
DRESULT FATFS_SD_SDIO_disk_read(BYTE *buff, DWORD sector, UINT count) 
{
	if (SD_ReadBlocks_DMA((uint32_t *)buff, (uint64_t) (sector * BLOCK_SIZE), BLOCK_SIZE, count) != MSD_OK) 
	{
		return RES_ERROR;
	}
	
	return RES_OK;
}

//=========================================================================================
DRESULT FATFS_SD_SDIO_disk_write(const BYTE *buff, DWORD sector, UINT count) 
{
	if (SD_WriteBlocks_DMA((uint32_t *)buff, (uint64_t) (sector * BLOCK_SIZE), BLOCK_SIZE, count) != MSD_OK) 
	{
		return RES_ERROR;
	}
	
	return RES_OK;
}


/*********************************************************/
/*      Low level functions for SDIO communciation       */
/*********************************************************/

static SD_HandleTypeDef 					uSdHandle;
static SD_CardInfo 								uSdCardInfo;

//=========================================================================================
/**
  * @brief  Initializes the SD card device.
  * @param  None
  * @retval SD status
  */
static uint8_t SD_Init(void) 
{ 
	uint8_t SD_state = MSD_OK;

	/* uSD device interface configuration */
	uSdHandle.Instance = SDMMC1;

	uSdHandle.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
  uSdHandle.Init.ClockBypass         = SDMMC_CLOCK_BYPASS_DISABLE;
  uSdHandle.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  uSdHandle.Init.BusWide             = SDMMC_BUS_WIDE_1B;
  uSdHandle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  uSdHandle.Init.ClockDiv            = SDMMC_TRANSFER_CLK_DIV;
	
	
	/* Msp SD Detect pin initialization */
	SD_Detect_MspInit();
	
	/* Check if SD card is present */
	if(SD_IsDetected() != SD_PRESENT) return MSD_ERROR_SD_NOT_PRESENT;

	/* Init GPIO and NVIC */
	SD_MspInit();
	
	/* HAL SD initialization */
  if(HAL_SD_Init(&uSdHandle, &uSdCardInfo) != SD_OK) SD_state = MSD_ERROR;
	
	/* Configure SD Bus width */
	if (SD_state == MSD_OK) 
	{
		/* Enable wide operation */
		if(HAL_SD_WideBusOperation_Config(&uSdHandle, SDMMC_BUS_WIDE_4B) != SD_OK) SD_state = MSD_ERROR; 
		else SD_state = MSD_OK;
	}
	return  SD_state;
}

//=========================================================================================
/**
  * @brief  Reads block(s) from a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to read 
  * @retval SD status
  */
uint8_t SD_ReadBlocks(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks) 
{
	if (HAL_SD_ReadBlocks(&uSdHandle, pData, ReadAddr, BlockSize, NumOfBlocks) != SD_OK) return MSD_ERROR;
	
	return MSD_OK;
}

//=========================================================================================
/**
  * @brief  Writes block(s) to a specified address in an SD card, in polling mode. 
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to write
  * @retval SD status
  */
uint8_t SD_WriteBlocks(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks) 
{
	if (HAL_SD_WriteBlocks(&uSdHandle, pData, WriteAddr, BlockSize, NumOfBlocks) != SD_OK) return MSD_ERROR;
  
	return MSD_OK;
}

//==============================================================================================================
/**
  * @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to read 
  * @retval SD status
  */
uint8_t SD_ReadBlocks_DMA(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks)
{
	uint8_t SD_state = MSD_OK;
	
	/* Read block(s) in DMA transfer mode */
	if (HAL_SD_ReadBlocks_DMA(&uSdHandle, pData, ReadAddr, BlockSize, NumOfBlocks) != SD_OK) SD_state = MSD_ERROR;

	/* Wait until transfer is complete */
	if (SD_state == MSD_OK) 
	{
		if (HAL_SD_CheckReadOperation(&uSdHandle, (uint32_t)SD_DATATIMEOUT) != SD_OK) SD_state = MSD_ERROR;
		else SD_state = MSD_OK;
	}

	return SD_state;
}

//==============================================================================================================
/**
  * @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to write 
  * @retval SD status
  */
uint8_t SD_WriteBlocks_DMA(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks) 
{
	uint8_t SD_state = MSD_OK;

	/* Write block(s) in DMA transfer mode */
	if (HAL_SD_WriteBlocks_DMA(&uSdHandle, pData, WriteAddr, BlockSize, NumOfBlocks) != SD_OK) SD_state = MSD_ERROR;

	/* Wait until transfer is complete */
	if (SD_state == MSD_OK) 
	{
		if(HAL_SD_CheckWriteOperation(&uSdHandle, (uint32_t)SD_DATATIMEOUT) != SD_OK) SD_state = MSD_ERROR;
		else SD_state = MSD_OK;
	}

	return SD_state;  
}

//==============================================================================================================
/**
  * @brief  Erases the specified memory area of the given SD card. 
  * @param  StartAddr: Start byte address
  * @param  EndAddr: End byte address
  * @retval SD status
  */
uint8_t SD_Erase(uint64_t StartAddr, uint64_t EndAddr) 
{
	if (HAL_SD_Erase(&uSdHandle, StartAddr, EndAddr) != SD_OK) return MSD_ERROR;
	
	return MSD_OK;
}

//=========================================================================================
static void SD_MspInit(void)
{
	static DMA_HandleTypeDef dma_rx_handle;
  static DMA_HandleTypeDef dma_tx_handle;
  GPIO_InitTypeDef gpio_init_struct;
	SD_HandleTypeDef *hsd = &uSdHandle;

  /* Enable SDIO clock */
  __HAL_RCC_SDMMC1_CLK_ENABLE();

	 /* Enable DMA2 clocks */
  __DMAx_TxRx_CLK_ENABLE();
	
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  
  /* Common GPIO configuration */
  gpio_init_struct.Mode      = GPIO_MODE_AF_PP;
  gpio_init_struct.Pull      = GPIO_PULLUP;
  gpio_init_struct.Speed     = GPIO_SPEED_HIGH;
  gpio_init_struct.Alternate = GPIO_AF12_SDMMC1;
  
  /* GPIOC configuration */
  gpio_init_struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
  HAL_GPIO_Init(GPIOC, &gpio_init_struct);

  /* GPIOD configuration */
  gpio_init_struct.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOD, &gpio_init_struct);

  /* NVIC configuration for SDIO interrupts */
  HAL_NVIC_SetPriority(SDMMC1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
	
	/* Configure DMA Rx parameters */
  dma_rx_handle.Init.Channel             = SD_DMAx_Rx_CHANNEL;
  dma_rx_handle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  dma_rx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
  dma_rx_handle.Init.MemInc              = DMA_MINC_ENABLE;
  dma_rx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dma_rx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  dma_rx_handle.Init.Mode                = DMA_PFCTRL;
  dma_rx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
  dma_rx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
  dma_rx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  dma_rx_handle.Init.MemBurst            = DMA_MBURST_INC4;
  dma_rx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;
  
  dma_rx_handle.Instance = SD_DMAx_Rx_STREAM;
  
  /* Associate the DMA handle */
  __HAL_LINKDMA(hsd, hdmarx, dma_rx_handle);
  
  /* Deinitialize the stream for new transfer */
  HAL_DMA_DeInit(&dma_rx_handle);
  
  /* Configure the DMA stream */
  HAL_DMA_Init(&dma_rx_handle);
  
  /* Configure DMA Tx parameters */
  dma_tx_handle.Init.Channel             = SD_DMAx_Tx_CHANNEL;
  dma_tx_handle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  dma_tx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
  dma_tx_handle.Init.MemInc              = DMA_MINC_ENABLE;
  dma_tx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dma_tx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  dma_tx_handle.Init.Mode                = DMA_PFCTRL;
  dma_tx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
  dma_tx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
  dma_tx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  dma_tx_handle.Init.MemBurst            = DMA_MBURST_INC4;
  dma_tx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;
  
  dma_tx_handle.Instance = SD_DMAx_Tx_STREAM;
  
  /* Associate the DMA handle */
  __HAL_LINKDMA(hsd, hdmatx, dma_tx_handle);
  
  /* Deinitialize the stream for new transfer */
  HAL_DMA_DeInit(&dma_tx_handle);
  
  /* Configure the DMA stream */
  HAL_DMA_Init(&dma_tx_handle); 
  
  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SD_DMAx_Rx_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(SD_DMAx_Rx_IRQn);
  
  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SD_DMAx_Tx_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(SD_DMAx_Tx_IRQn);
}

//===================================================================================
static void SD_Detect_MspInit(void)
{
	GPIO_InitTypeDef gpio_struct;
	
	SD_DETECT_GPIO_CLK_ENABLE();
	
	/* GPIO configuration in input for uSD_Detect signal */
  gpio_struct.Pin       = SD_DETECT_PIN;
  gpio_struct.Mode      = GPIO_MODE_INPUT;
  gpio_struct.Pull      = GPIO_PULLUP;
  gpio_struct.Speed     = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(SD_DETECT_GPIO_PORT, &gpio_struct);
}

//===================================================================================
void SD_GetCardInfo(HAL_SD_CardInfoTypedef *CardInfo) 
{
	/* Get SD card Information */
	HAL_SD_Get_CardInfo(&uSdHandle, CardInfo);
}

//===================================================================================
HAL_SD_TransferStateTypedef SD_GetStatus(void) 
{
	return HAL_SD_GetStatus(&uSdHandle);
}

//================================================================================================
void SDMMC1_IRQHandler(void)
{
	HAL_SD_IRQHandler(&uSdHandle);
}

//================================================================================================
/**
  * @brief  This function handles DMA2 Stream 3 interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream3_IRQHandler(void) {
	HAL_DMA_IRQHandler(uSdHandle.hdmarx);
}

//================================================================================================
/**
  * @brief  This function handles DMA2 Stream 6 interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream6_IRQHandler(void) {
	HAL_DMA_IRQHandler(uSdHandle.hdmatx); 
}
