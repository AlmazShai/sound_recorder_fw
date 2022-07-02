#ifndef BOARD_SD_H_
#define BOARD_SD_H_

#include "err_code.h"

#include <stdbool.h>
#include <stdint.h>

#define BOARD_SD_DEFAULT_BLOCK_SIZE 512  // SD card internal block size
#define BOARD_SD_OP_TIMEOUT_MS      1000 // SDIO operation timeout

/**
 * @brief SD card informaiton structure typedef.
 *
 */
typedef struct
{
    uint32_t log_block_size;
    uint32_t log_block_n;
} board_sd_info_t;

/**
 * @brief   SDIO peripheral and SD card initializing.
 *
 * @retval  CODE_SUCCESS      - Initalized successfully
 * @retval  CODE_ERR_INTERNAL - SDK error, check sd card connection
 */
ret_code_t board_sd_init(void);

/**
 * @brief   Read blocks from sd card in blocking mode.
 *
 * @note    Only 512 byte blocks supported.
 *
 * @param   p_data      - Pointer to destination buffer
 * @param   read_addr   - Block address to read
 * @param   block_n     - Amount of blocks to read
 *
 * @retval  CODE_SUCCESS      - Blocks read success
 * @retval  CODE_ERR_INTERNAL - Sd card connection error
 * @retval  CODE_ERR_TIMEOUT  - Read timeout occurred
 */
ret_code_t board_sd_read_block(uint8_t* p_data, uint32_t read_addr, uint32_t block_n);

/**
 * @brief   Write blocks to sd card in blocking mode.
 *
 * @note    Only 512 byte blocks supported.
 *
 * @param   p_data      - Pointer to buffer to be written
 * @param   write_addr  - Block address to write
 * @param   block_n     - Amount of blocks to write
 *
 * @retval  CODE_SUCCESS      - Blocks write success
 * @retval  CODE_ERR_INTERNAL - Sd card connection error
 * @retval  CODE_ERR_TIMEOUT  - Write timeout occurred
 */
ret_code_t board_sd_write_block(uint8_t* p_data, uint32_t write_addr, uint32_t block_n);

/**
 * @brief   Read blocks from sd card in interrupt mode.
 *
 * @note    Only 512 byte blocks supported.
 *
 * @param   p_data      - Pointer to destination buffer
 * @param   read_addr   - Block address to read
 * @param   block_n     - Amount of blocks to read
 *
 * @retval  CODE_SUCCESS      - Blocks read success
 * @retval  CODE_ERR_INTERNAL - Sd card connection error
 * @retval  CODE_ERR_BUSY     - SD card is in transfer process
 */
ret_code_t board_sd_read_block_it(uint8_t* p_data, uint32_t read_addr, uint32_t block_n);

/**
 * @brief   Write blocks to sd card in interrupt mode.
 *
 * @note    Only 512 byte blocks supported.
 *
 * @param   p_data      - Pointer to buffer to be written
 * @param   write_addr  - Block address to write
 * @param   block_n     - Amount of blocks to write
 *
 * @retval  CODE_SUCCESS      - Blocks write success
 * @retval  CODE_ERR_INTERNAL - Sd card connection error
 * @retval  CODE_ERR_BUSY     - SD card is in transfer process
 */
ret_code_t board_sd_write_block_it(uint8_t* p_data, uint32_t read_addr, uint32_t block_n);

/**
 * @brief   Erase sd card blocks in blocking mode.
 *
 * @param   start_addr - block start address to erase
 * @param   end_addr   - Block end address to erase
 *
 * @retval  CODE_SUCCESS      - Operation success
 * @retval  CODE_ERR_INTERNAL - Sd card connection error
 * @retval  CODE_ERR_TIMEOUT  - Operation timeout
 */
ret_code_t board_sd_erase(uint32_t start_addr, uint32_t end_addr);

/**
 * @brief   Erase full sd card in blocking mode.
 *
 * @retval  CODE_SUCCESS      - Operation success
 * @retval  CODE_ERR_INTERNAL - Sd card connection error
 * @retval  CODE_ERR_TIMEOUT  - Operation timeout
 */
ret_code_t board_sd_erase_all(void);

/**
 * @brief   Check SD card busy state.
 *
 * @return  true    - busy
 * @return  false   - not busy
 */
bool board_sd_is_busy(void);

/**
 * @brief   Get sd card info
 *
 * @param p_info    - Pointer to destination struct
 *
 * @retval  CODE_SUCCESS      - Operation success
 * @retval  CODE_ERR_INTERNAL - Sd card connection error
 */
ret_code_t board_sd_get_info(board_sd_info_t* p_info);

#endif // BOARD_SD_H_
