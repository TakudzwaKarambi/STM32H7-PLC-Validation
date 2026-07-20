#ifndef CLT01_38SQ7_H
#define CLT01_38SQ7_H

#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    SPI_HandleTypeDef *hspi;

    /* Chip Select Pin */
    GPIO_TypeDef *CS_Port;
    uint16_t      CS_Pin;

    /* New Hardware Control & Diagnostic Pins */
    GPIO_TypeDef *EN_Port;     // DI_EN
    uint16_t      EN_Pin;

    GPIO_TypeDef *GOOD_Port;   // DI_GOOD
    uint16_t      GOOD_Pin;

    GPIO_TypeDef *FAULT_Port;  // DI_FAULT
    uint16_t      FAULT_Pin;

} CLT01_Handle_t;

typedef struct
{
    uint8_t  Inputs;
    bool     UnderVoltage;
    bool     OverTemperature;
    bool     StopBitError;
    bool     ParityError;
    uint16_t RawFrame;
} CLT01_Status_t;

HAL_StatusTypeDef CLT01_Init(CLT01_Handle_t *hclt);
HAL_StatusTypeDef CLT01_ReadStatus(CLT01_Handle_t *hclt, CLT01_Status_t *status);
bool CLT01_IsAlive(const CLT01_Status_t *status);
HAL_StatusTypeDef CLT01_TestCommunication(CLT01_Handle_t *hclt, uint16_t *rawFrame);

#endif
