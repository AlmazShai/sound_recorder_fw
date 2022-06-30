#include "rec_storage.h"

#include "stddef.h"
#include "board_sd.h"

static uint8_t  samples_buf[REC_STORAGE_BUFF_SIZE_BYTES];
static uint16_t buff_write_idx = 0;
static uint32_t write_addr     = 0;

ret_code_t rec_storage_init(void)
{
    ret_code_t err_code;
    err_code = board_sd_init();
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }
    err_code = board_sd_erase_all();
    return err_code;
}

ret_code_t rec_storage_start_saving(void)
{
    buff_write_idx = 0;
    return CODE_SUCCESS;
}

ret_code_t rec_storage_save_samples(uint16_t* p_data, uint16_t len_bytes)
{
    ret_code_t err_code;

    for (uint16_t read_idx = 0; read_idx < len_bytes; read_idx += sizeof(uint16_t))
    {
        samples_buf[buff_write_idx] = *((uint8_t*)p_data + read_idx);
        buff_write_idx++;
        if (buff_write_idx >= REC_STORAGE_BUFF_SIZE_BYTES)
        {
            err_code = board_sd_write_block((uint32_t*)p_data, write_addr, 1);
            if (err_code != CODE_SUCCESS)
            {
                return err_code;
            }
            buff_write_idx = 0;
            write_addr++;
        }
    }
    return CODE_SUCCESS;
}

ret_code_t rec_storage_stop_saving(void) {
	return CODE_SUCCESS
			;
}
