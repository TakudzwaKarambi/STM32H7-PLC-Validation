#include "current_limiter.h"
#include "spi.h"
#include "main.h"

static CLT01_Handle_t g_clt;

HAL_StatusTypeDef CurrentLimiter_Init(void)
{
    g_clt.hspi    = &hspi1;

    /* Hardwired assignments matching your board layout */
    g_clt.CS_Port = GPIOA; // PA4
    g_clt.CS_Pin  = GPIO_PIN_4;

    /* Assigning the additional hardware control lines */
    g_clt.EN_Port    = DI_EN_GPIO_Port;
    g_clt.EN_Pin     = DI_EN_Pin;

    g_clt.GOOD_Port  = DI_GOOD_GPIO_Port;
    g_clt.GOOD_Pin   = DI_GOOD_Pin;

    g_clt.FAULT_Port = DI_FAULT_GPIO_Port;
    g_clt.FAULT_Pin  = DI_FAULT_Pin;

    return CLT01_Init(&g_clt);
}

HAL_StatusTypeDef CurrentLimiter_Read(CLT01_Status_t *status)
{
    return CLT01_ReadStatus(&g_clt, status);
}

/* Expose handle internally for test sweeps */
CLT01_Handle_t* CurrentLimiter_GetHandle(void)
{
    return &g_clt;
}
