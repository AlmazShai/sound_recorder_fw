#ifndef ERR_CODE_H_
#define ERR_CODE_H_

#include <stdint.h>

typedef uint32_t ret_code_t;

#define RET_CODE_BASE_NUM      0u

#define CODE_SUCCESS           (RET_CODE_BASE_NUM)
#define CODE_ERR_INVALID_PARAM (RET_CODE_BASE_NUM + 1u)
#define CODE_ERR_INVALID_STATE (RET_CODE_BASE_NUM + 2u)
#define CODE_ERR_INTERNAL      (RET_CODE_BASE_NUM + 3u)
#define CODE_ERR_BUSY          (RET_CODE_BASE_NUM + 4u)
#define CODE_ERR_TIMEOUT       (RET_CODE_BASE_NUM + 5u)

#endif // ERR_CODE_H_