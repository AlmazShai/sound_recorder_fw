#ifndef REC_STORAGE_H_
#define REC_STORAGE_H_

#include "err_code.h"

ret_code_t rec_storage_init(void);

ret_code_t rec_storage_start_saving(void);

ret_code_t rec_storage_save_data(uint16_t p_data, uint16_t len);

ret_code_t rec_storage_stop_saving(void);

#endif // REC_STORAGE_H_