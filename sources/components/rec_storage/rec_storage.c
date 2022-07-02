#include "rec_storage.h"

#include "stddef.h"
#include "board_sd.h"

static uint8_t  samples_buff1[REC_STORAGE_BUFF_SIZE_BYTES]; // internal buffer
static uint8_t  samples_buff2[REC_STORAGE_BUFF_SIZE_BYTES]; // second internal buffer
static uint16_t buff_write_idx    = 0;                      // internal buffer index
static uint32_t sd_block_addr     = 0;                      // sd card block address
static uint32_t sd_max_block_addr = 0;                      // sd card block size
static uint8_t * p_curr_buff = samples_buff1;			// pointer to current saving buffer

static void switch_buffer(void)
{
    if (p_curr_buff == samples_buff1)
    {
    	p_curr_buff = samples_buff2;
    }
    else
    {
    	p_curr_buff = samples_buff2;
    }
    buff_write_idx = 0;
}

static bool is_sd_card_full(void)
{
    if (sd_block_addr >= sd_max_block_addr)
    {
        return true;
    }
    return false;
}

ret_code_t rec_storage_init(void)
{
    ret_code_t err_code;
    // initialize sd card operation module 
    err_code = board_sd_init();
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }

    board_sd_info_t sd_info = {0};

    // keep sd card blocks amount
    err_code = board_sd_get_info(&sd_info);
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }
    sd_max_block_addr = sd_info.log_block_n;

    // erase sd card
    err_code = board_sd_erase_all();
    return err_code;
}

ret_code_t rec_storage_start_saving(void)
{
    buff_write_idx       = 0;
    return CODE_SUCCESS;
}

ret_code_t rec_storage_save_samples(uint16_t* p_data, uint16_t len_bytes)
{
    ret_code_t err_code;

    if (is_sd_card_full() == true)
    {
        return CODE_ERR_NO_MEM;
    }

    for (uint16_t read_idx = 0; read_idx < len_bytes; read_idx++)
    {
    	p_curr_buff[buff_write_idx] = *((uint8_t*)p_data + read_idx);
        buff_write_idx++;
        if (buff_write_idx >= REC_STORAGE_BUFF_SIZE_BYTES)
        {
            err_code = board_sd_write_block_it(p_curr_buff, sd_block_addr, 1);
            if (err_code != CODE_SUCCESS)
            {
                return err_code;
            }
            switch_buffer();
            sd_block_addr++;
        }
    }
    return CODE_SUCCESS;
}

ret_code_t rec_storage_stop_saving(void)
{
    return CODE_SUCCESS;
}
