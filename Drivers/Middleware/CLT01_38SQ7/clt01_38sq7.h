#ifndef CLT01_38SQ7_H
#define CLT01_38SQ7_H

#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    SPI_HandleTypeDef *hspi;

    /* SPI Chip Select */
    GPIO_TypeDef *CS_Port;
    uint16_t CS_Pin;

    /* Device Enable */
    GPIO_TypeDef *EN_Port;
    uint16_t EN_Pin;

    /* Optional Hardware Diagnostic Pins */
    GPIO_TypeDef *GOOD_Port;
    uint16_t GOOD_Pin;

    GPIO_TypeDef *FAULT_Port;
    uint16_t FAULT_Pin;

} CLT01_Handle_t;


/*=========================================================
 * Decoded SPI Status Frame
 *========================================================*/
typedef struct
{
    /* Raw SPI frame */
    uint16_t RawFrame;

    /* Input bitmap (IN1..IN8) */
    uint8_t Inputs;

    /* Individual input states */
    uint8_t Channel[8];

    /* Device diagnostics */
    uint8_t SupplyVoltageAlarm;
    uint8_t OverTemperatureAlarm;

    /* Frame diagnostics */
    uint8_t ParityValid;
    uint8_t StopBitsValid;
   uint8_t FrameValid;

    /* Hardware diagnostics */
    uint8_t GoodPinState;
    uint8_t FaultPinState;

    /* Overall driver status */
    uint8_t DeviceAlive;

} CLT01_Status_t;


/* Driver Functions */
HAL_StatusTypeDef CLT01_Init(CLT01_Handle_t *hclt);

HAL_StatusTypeDef CLT01_ReadStatus(
        CLT01_Handle_t *hclt,
        CLT01_Status_t *status);

HAL_StatusTypeDef CLT01_TestCommunication(
        CLT01_Handle_t *hclt,
        uint16_t *rawFrame);

bool CLT01_IsAlive(
        const CLT01_Status_t *status);

#endif
