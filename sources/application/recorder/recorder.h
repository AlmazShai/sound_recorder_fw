#ifndef RECORDER_H_
#define RECORDER_H_

#include "err_code.h"

typedef enum
{
    RECORDER_STATE_IDLE,
    RECORDER_STATE_RUN,
} recorder_state_t;

#define RECORDER_BUFF_SIZE 200

ret_code_t recorder_init(void);

ret_code_t recorder_start(void);

ret_code_t recorder_stop(void);

void recorder_process(void);

#endif // RECORDER_H_
