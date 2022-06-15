#ifndef BOARD_MIC_H_
#define BOARD_MIC_H_

#include "err_code.h"

typedef enum
{
    BOARD_MIC_EVT_HALF_TR_COMPLETE,
    BOARD_MIC_EVT_TR_COMPLETE,
    BOARD_MIC_EVT_ERROR,
} board_mic_evt_t;

typedef struct
{
    uint16_t * p_buff;
    uint16_t buff_size;
} board_mic_puff_cfg_t;


typedef void (*board_mic_evt_cb_t)(board_mic_evt_t evt);

ret_code_t board_mic_init(board_mic_puff_cfg_t *cfg);

void board_mic_set_cb(board_mic_evt_cb_t cb);

ret_code_t board_mic_start_stream(void);

ret_code_t board_mic_stop_stream(void);

#endif // BOARD_MIC_H_