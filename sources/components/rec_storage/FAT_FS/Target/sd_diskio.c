#include "sd_diskio.h"
#include "ff_gen_drv.h"
#include "board_sd.h"

#define SD_DEFAULT_BLOCK_SIZE 512

static volatile DSTATUS Stat = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
static DSTATUS SD_CheckStatus(BYTE lun);
DSTATUS        SD_initialize(BYTE);
DSTATUS        SD_status(BYTE);
DRESULT        SD_read(BYTE, BYTE*, DWORD, UINT);
#if _USE_WRITE == 1
DRESULT SD_write(BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE, BYTE, void*);
#endif /* _USE_IOCTL == 1 */

const Diskio_drvTypeDef SD_Driver = {
    SD_initialize, SD_status, SD_read,
#if _USE_WRITE == 1
    SD_write,
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
    SD_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* USER CODE BEGIN beforeFunctionSection */
/* can be used to modify / undefine following code or add new code */
/* USER CODE END beforeFunctionSection */

/* Private functions ---------------------------------------------------------*/

static DSTATUS SD_CheckStatus(BYTE lun)
{
    Stat = STA_NOINIT;

    if (board_sd_is_busy() == false)
    {
        Stat &= ~STA_NOINIT;
    }

    return Stat;
}

/**
 * @brief  Initializes a Drive
 * @param  lun : not used
 * @retval DSTATUS: Operation status
 */
DSTATUS SD_initialize(BYTE lun)
{
    Stat = STA_NOINIT;

#if !defined(DISABLE_SD_INIT)

    if (board_sd_init() == CODE_SUCCESS)
    {
        Stat = SD_CheckStatus(lun);
    }

#else
    Stat = SD_CheckStatus(lun);
#endif

    return Stat;
}

/**
 * @brief  Gets Disk Status
 * @param  lun : not used
 * @retval DSTATUS: Operation status
 */
DSTATUS SD_status(BYTE lun)
{
    return SD_CheckStatus(lun);
}

/* USER CODE BEGIN beforeReadSection */
/* can be used to modify previous code / undefine following code / add new code */
/* USER CODE END beforeReadSection */
/**
 * @brief  Reads Sector(s)
 * @param  lun : not used
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: Operation result
 */

DRESULT SD_read(BYTE lun, BYTE* buff, DWORD sector, UINT count)
{
    DRESULT res = RES_ERROR;

    if (board_sd_read_block((uint32_t*)buff, (uint32_t)(sector), count) == CODE_SUCCESS)
    {
        res = RES_OK;
    }

    return res;
}

/* USER CODE BEGIN beforeWriteSection */
/* can be used to modify previous code / undefine following code / add new code */
/* USER CODE END beforeWriteSection */
/**
 * @brief  Writes Sector(s)
 * @param  lun : not used
 * @param  *buff: Data to be written
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to write (1..128)
 * @retval DRESULT: Operation result
 */
#if _USE_WRITE == 1

DRESULT SD_write(BYTE lun, const BYTE* buff, DWORD sector, UINT count)
{
    DRESULT res = RES_ERROR;

    if (board_sd_write_block((uint32_t*)buff, (uint32_t)(sector), count) == CODE_SUCCESS)
    {
        res = RES_OK;
    }

    return res;
}
#endif /* _USE_WRITE == 1 */

/* USER CODE BEGIN beforeIoctlSection */
/* can be used to modify previous code / undefine following code / add new code */
/* USER CODE END beforeIoctlSection */
/**
 * @brief  I/O control operation
 * @param  lun : not used
 * @param  cmd: Control code
 * @param  *buff: Buffer to send/receive control data
 * @retval DRESULT: Operation result
 */
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE lun, BYTE cmd, void* buff)
{
    DRESULT         res = RES_ERROR;
    board_sd_info_t CardInfo;

    if (Stat & STA_NOINIT)
        return RES_NOTRDY;

    switch (cmd)
    {
        /* Make sure that no pending write process */
        case CTRL_SYNC:
            res = RES_OK;
            break;

        /* Get number of sectors on the disk (DWORD) */
        case GET_SECTOR_COUNT:
            board_sd_get_info(&CardInfo);
            *(DWORD*)buff = CardInfo.log_block_n;
            res           = RES_OK;
            break;

        /* Get R/W sector size (WORD) */
        case GET_SECTOR_SIZE:
            board_sd_get_info(&CardInfo);
            *(WORD*)buff = CardInfo.log_block_size;
            res          = RES_OK;
            break;

        /* Get erase block size in unit of sector (DWORD) */
        case GET_BLOCK_SIZE:
            board_sd_get_info(&CardInfo);
            *(DWORD*)buff = CardInfo.log_block_size / SD_DEFAULT_BLOCK_SIZE;
            res           = RES_OK;
            break;

        default:
            res = RES_PARERR;
    }

    return res;
}
#endif /* _USE_IOCTL == 1 */
