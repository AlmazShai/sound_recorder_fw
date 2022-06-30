#ifndef RECORDER_H_
#define RECORDER_H_

#include "err_code.h"


#define RECORDER_PDM_BUFF_SIZE (128u)
#define RECORDER_PCM_BUFF_SIZE                                                           \
    (RECORDER_PDM_BUFF_SIZE * 2) // with minimum decimation factor 16
                                 // and doubling samples for stereo sound, twice
                                 // size buffer are required

typedef enum
{
    RECORDER_STATE_IDLE,
    RECORDER_STATE_RUN,
} recorder_state_t;

ret_code_t recorder_init(void);

ret_code_t recorder_start(void);

ret_code_t recorder_stop(void);

void recorder_process(void);

recorder_state_t recorder_get_state(void);

#endif // RECORDER_H_
