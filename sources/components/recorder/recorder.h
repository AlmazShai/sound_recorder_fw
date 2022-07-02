#ifndef RECORDER_H_
#define RECORDER_H_

#include "err_code.h"

#define RECORDER_PDM_BUFF_SIZE (256u)
#define RECORDER_PCM_BUFF_SIZE                                                           \
    (RECORDER_PDM_BUFF_SIZE * 2) // with minimum decimation factor 16
                                 // and doubling samples for stereo sound, twice
                                 // size buffer are required

/**
 * @brief Recorder module states typedef.
 *
 */
typedef enum
{
    RECORDER_STATE_IDLE, // Ready  to recording
    RECORDER_STATE_RUN,  // Recording in progress
} recorder_state_t;

/**
 * @brief   Initialized recorder module
 *
 * @retval  CODE_SUCCESS     - initialize success
 * @retval  CODE_ERR_SD_CARD - Error while sd card operation
 * @retval  CODE_ERR_MIC     - Error while microphone operation
 */
ret_code_t recorder_init(void);

/**
 * @brief   Start recording process.
 * 
 * @retval  CODE_SUCCESS     - Success
 * @retval  CODE_ERR_SD_CARD - Error while sd card operation
 * @retval  CODE_ERR_MIC     - Error while microphone operation
 */
ret_code_t recorder_start(void);

/**
 * @brief   Stop recording process.
 * 
 * @retval  CODE_SUCCESS     - initialize success
 * @retval  CODE_ERR_SD_CARD - Error while sd card operation
 * @retval  CODE_ERR_MIC     - Error while microphone operation
 */
ret_code_t recorder_stop(void);

/**
 * @brief   Run recording process.
 * 
 */
void recorder_process(void);

/**
 * @brief Get recording state
 * 
 * @return recorder_state_t 
 */
recorder_state_t recorder_get_state(void);

#endif // RECORDER_H_
