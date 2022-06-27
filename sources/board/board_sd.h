#ifndef BOARD_SD_H_
#define BOARD_SD_H_

#include "err_code.h"

#include <stdbool.h>
#include <stdint.h>

#define BOARD_SD_DEFAULT_BLOCK_SIZE 512
#define BOARD_SD_OP_TIMEOUT_MS      5000

typedef struct
{
    uint32_t log_block_size;
    uint32_t log_block_n;
} board_sd_info_t;

ret_code_t board_sd_init(void);

ret_code_t board_sd_read_block(uint32_t* p_data, uint32_t read_addr, uint32_t block_n);

ret_code_t board_sd_write_block(uint32_t* p_data, uint32_t WriteAddr, uint32_t block_n);

ret_code_t board_sd_erase(uint32_t start_addr, uint32_t end_addr);

ret_code_t board_sd_erase_all(void);

bool board_sd_is_busy(void);

ret_code_t board_sd_get_info(board_sd_info_t* p_info);

#endif // BOARD_SD_H_
