#include "AK_stm32_fatfs.h"

#include "ff.h"
#include "diskio.h"

//================================= FatFs functions structures init ===========================================

FRESULT AK_FATFS_GetDriveSize(char* str, AK_FATFS_Size_t* SizeStruct)
{
	FATFS *fs;
  DWORD fre_clust;
	FRESULT res;

  /* Get volume information and free clusters of drive */
  if ((res = f_getfree(str, &fre_clust, &fs)) != FR_OK) return res;

  /* Get total sectors and free sectors */
  SizeStruct->Total = (fs->n_fatent - 2) * fs->csize * 0.5;
  SizeStruct->Free = fre_clust * fs->csize * 0.5;
	
	/* Return OK */
	return FR_OK;
}
