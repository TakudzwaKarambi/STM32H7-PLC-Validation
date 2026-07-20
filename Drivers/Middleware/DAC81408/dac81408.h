#ifndef DAC81408_H
#define DAC81408_H

#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#define DAC81408_NUM_CHANNELS    8

/* Registers */
#define DAC81408_REG_DEVICEID      0x01
#define DAC81408_REG_STATUS        0x02
#define DAC81408_REG_GENCONFIG     0x04
#define DAC81408_REG_SYNCCONFIG    0x06
#define DAC81408_REG_DACPWDWN      0x09
#define DAC81408_REG_DACRANGE0     0x0B
#define DAC81408_REG_DACRANGE1     0x0C
#define DAC81408_REG_TRIGGER       0x0E

#define DAC81408_REG_DAC0          0x14
#define DAC81408_REG_DAC1          0x15
#define DAC81408_REG_DAC2          0x16
#define DAC81408_REG_DAC3          0x17
#define DAC81408_REG_DAC4          0x18
#define DAC81408_REG_DAC5          0x19
#define DAC81408_REG_DAC6          0x1A
#define DAC81408_REG_DAC7          0x1B

typedef enum
{
    DAC81408_RANGE_PM2V5 = 0,
    DAC81408_RANGE_PM5V,
    DAC81408_RANGE_PM10V,
    DAC81408_RANGE_PM20V,

    DAC81408_RANGE_0_5V,
    DAC81408_RANGE_0_10V,
    DAC81408_RANGE_0_20V,
    DAC81408_RANGE_0_40V

} DAC81408_Range_t;

typedef struct
{
    bool initialized;
    bool spiHealthy;

    bool tempAlarm;
    bool crcAlarm;
    bool busy;

    uint32_t commErrors;

} DAC81408_Status_t;

typedef struct
{
    SPI_HandleTypeDef *spiHandle;

    GPIO_TypeDef *csPort;
    uint16_t csPin;

    GPIO_TypeDef *resetPort;
    uint16_t resetPin;

    GPIO_TypeDef *ldacPort;
    uint16_t ldacPin;

    GPIO_TypeDef *clrPort;
    uint16_t clrPin;

    GPIO_TypeDef *alarmPort;
    uint16_t alarmPin;

    DAC81408_Status_t status;

} DAC81408_t;

/* API */

HAL_StatusTypeDef DAC81408_Init(
    DAC81408_t *dac,
    SPI_HandleTypeDef *spiHandle);

HAL_StatusTypeDef DAC81408_Reset(
    DAC81408_t *dac);

HAL_StatusTypeDef DAC81408_SetRange(
    DAC81408_t *dac,
    uint8_t channel,
    DAC81408_Range_t range);

HAL_StatusTypeDef DAC81408_WriteChannel(
    DAC81408_t *dac,
    uint8_t channel,
    uint16_t value);

HAL_StatusTypeDef DAC81408_WriteAll(
    DAC81408_t *dac,
    uint16_t value[8]);

HAL_StatusTypeDef DAC81408_UpdateOutputs(
    DAC81408_t *dac);

HAL_StatusTypeDef DAC81408_ReadStatus(
    DAC81408_t *dac);

HAL_StatusTypeDef DAC81408_ReadRegister(
    DAC81408_t *dac,
    uint8_t reg,
    uint16_t *value);

HAL_StatusTypeDef DAC81408_WriteRegister(
    DAC81408_t *dac,
    uint8_t reg,
    uint16_t value);

const DAC81408_Status_t*
DAC81408_GetStatus(
    DAC81408_t *dac);

#endif
