#ifndef BOARD_MIC_H_
#define BOARD_MIC_H_

#include "err_code.h"

#define BOARD_MIC_I2S_SCK_FREQ_HZ (1024000u) // SCK pin clock frequency

/**
 * @brief Microphone events types.
 *
 */
typedef enum
{
    BOARD_MIC_EVT_HALF_TR_COMPLETE, // Half buffer transfer completed
    BOARD_MIC_EVT_TR_COMPLETE,      // Buffer transfer completed
    BOARD_MIC_EVT_ERROR,            // i2s peripheral error
} board_mic_evt_t;

/**
 * @brief Buffer configuration parameters typedef.
 *
 */
typedef struct
{
    uint16_t* p_buff;
    uint16_t  buff_size;
} board_mic_puff_cfg_t;

/**
 * @brief Microphone event callback function typedef.
 *
 */
typedef void (*board_mic_evt_cb_t)(board_mic_evt_t evt);

/**
 * @brief   Initialize microphone peripheral.
 *
 * @param   cfg - Pointer to buffer configuration parameters.
 *
 * @retval  CODE_SUCCESS - Initalized success
 * @retval  CODE_ERR_INTERNAL - SDK error occurred
 */
ret_code_t board_mic_init(board_mic_puff_cfg_t* cfg);

/**
 * @brief   Set events callback function.
 *
 * @param   cb  - Pointer to callback function.
 */
void board_mic_set_cb(board_mic_evt_cb_t cb);

/**
 * @brief   Start microphone samples streaming.
 *
 * @retval  CODE_SUCCESS            - Stream started successfully.
 * @retval  CODE_ERR_INTERNAL       - SDK error occurred
 * @retval  CODE_ERR_INVALID_STATE  - Not initialized properly
 */
ret_code_t board_mic_start_stream(void);

/**
 * @brief   Stop microphone data streaming.
 *
 * @retval  CODE_SUCCESS            - Stoped successfully.
 * @retval  CODE_ERR_INTERNAL       - SDK error occurred.
 */
ret_code_t board_mic_stop_stream(void);

#endif // BOARD_MIC_H_