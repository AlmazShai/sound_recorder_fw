#ifndef REC_STORAGE_H_
#define REC_STORAGE_H_

#include "err_code.h"

#define REC_STORAGE_BUFF_SIZE_BYTES 512

ret_code_t rec_storage_init(void);

ret_code_t rec_storage_start_saving(void);

ret_code_t rec_storage_save_samples(uint16_t* p_data, uint16_t len_bytes);

ret_code_t rec_storage_stop_saving(void);

#endif // REC_STORAGE_H_