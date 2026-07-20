#ifndef CURRENT_LIMITER_H
#define CURRENT_LIMITER_H

#include "clt01_38sq7.h"

HAL_StatusTypeDef CurrentLimiter_Init(void);

HAL_StatusTypeDef CurrentLimiter_Read(
    CLT01_Status_t *status);

#endif
