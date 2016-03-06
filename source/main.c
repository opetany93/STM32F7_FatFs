#include "init.h"

// =============================================== FatFs includes ==============================================
#include "ff.h"
#include "diskio.h"
#include "fatfs_sd_sdio.h"
#include "AK_stm32_fatfs.h"

//===============================================================================================================
/* Fatfs structure */
FATFS FS;
FIL file;
FRESULT fres;
DIR folder;

AK_FATFS_Size_t CardSize;	//struct for size

//===============================================================================================================
int main (void)
{
	init();
	
	if (f_mount(&FS, "SD:", 1) == FR_OK) 
	{
		f_mkdir("SD:/FatFs_TEST");
			/* Try to open file */
			if (f_open(&file, "SD:/FatFs_TEST/test.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK) 
			{
				/* Read SDCARD size */
				AK_FATFS_GetDriveSize("SD:", &CardSize);
			
				/* Format string and write total card size to file */
				f_printf(&file, "Total card size: %u kBytes\n", CardSize.Total);
			
				/* Format string for free card size and write free card size to file */
				f_printf(&file, "Free card size:  %u kBytes\n", CardSize.Free);
			
				/* Close file */
				f_close(&file);
			
				/* Turn led ON */
				ledOn();
			}	
		/* Unmount SDCARD */
			f_mount(NULL, "SD:", 1);
	}

	while(1);
}
