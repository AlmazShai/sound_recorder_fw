#include "rec_storage.h"

#include "stddef.h"
#include "board_sd.h"

static uint16_t samples_buf[5120];
static uint32_t write_addr = 0;

ret_code_t rec_storage_init(void)
{
    ret_code_t err_code;
    err_code = board_sd_init();
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }
    err_code = board_sd_erase();
    return err_code;
}

ret_code_t rec_storage_start_saving(void)
{
    write_addr = 0;
}

ret_code_t rec_storage_save_data(uint16_t* p_data, uint16_t len_bytes)
{
    return board_sd_write_block((uint32_t*)p_data, write_addr, 0);
}

ret_code_t rec_storage_stop_saving(void) {}
