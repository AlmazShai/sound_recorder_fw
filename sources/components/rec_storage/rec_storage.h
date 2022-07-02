/**
 * @file    rec_storage.h
 * @author  Almaz Shaidullin (almazshai@gmail.com)
 * @brief   Record storage library,
 * @details This library used to save audio samples to sd card memory.
 * @version 0.1
 * @date    2022-07-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef REC_STORAGE_H_
#define REC_STORAGE_H_

#include "err_code.h"

#define REC_STORAGE_BUFF_SIZE_BYTES 512

/**
 * @brief   This fuction initialize recorder storage.
 *
 * @details While initializing all sd card card will erased.
 *
 * @retval  CODE_SUCCESS - Initalized successfully
 * @retval  CODE_ERR_INTERNAL - sd card communication error
 */
ret_code_t rec_storage_init(void);

/**
 * @brief  This function start saving the new record.
 *
 * @retval CODE_SUCCESS - Saving new record start success
 */
ret_code_t rec_storage_start_saving(void);

/**
 * @brief   This fuction save new samples to external memory.
 *
 * @param   p_data    Pointer to samples
 * @param   len_bytes SIze of samples in byte unit
 *
 * @retval  CODE_SUCCESS - New samples was successfully saved
 * @retval  CODE_ERR_INTERNAL - Sd card communication error
 */
ret_code_t rec_storage_save_samples(uint16_t* p_data, uint16_t len_bytes);

/**
 * @brief   This function stops the record saving
 *
 * @retval  CODE_SUCCESS - Record saving was successfully stoped.
 */
ret_code_t rec_storage_stop_saving(void);

#endif // REC_STORAGE_H_