#ifndef BOARD_H_
#define BOARD_H_

#include "err_code.h"

/**
 * @brief   Initialize board clock and ect.
 *
 * @retval  CODE_SUCCESS        - Initialize success
 * @retval  CODE_ERR_INTERNAL   - SDK error
 */
ret_code_t board_init(void);

#endif // BOARD_H_
