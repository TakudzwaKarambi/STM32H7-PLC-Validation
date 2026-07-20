#ifndef __ANALOG_INPUT_H
#define __ANALOG_INPUT_H
#include <stdbool.h>
#include "stm32h7xx_hal.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*=========================================================
  STATUS
=========================================================*/

typedef enum
{
    ANALOG_INPUT_OK = 0,
    ANALOG_INPUT_ERROR,
    ANALOG_INPUT_TIMEOUT,
    ANALOG_INPUT_NOT_IMPLEMENTED
}
ANALOG_INPUT_StatusTypeDef;

/*=========================================================
  CONFIGURATION
=========================================================*/

typedef struct
{
    uint8_t channelCount;
}
ANALOG_INPUT_InitTypeDef;

/*=========================================================
  DRIVER INTERFACE
=========================================================*/

typedef struct
{
    ANALOG_INPUT_StatusTypeDef (*Init)(
        ANALOG_INPUT_InitTypeDef *init);

    void (*DeInit)(void);

    ANALOG_INPUT_StatusTypeDef (*ReadRaw)(
        uint16_t *data);

    ANALOG_INPUT_StatusTypeDef (*GetStatus)(
        uint8_t *status);

}
ANALOG_INPUT_DrvTypeDef;

/*=========================================================
  BSP API
=========================================================*/
/* Diagnostic function declaration */
bool ANALOG_INPUT_TestSPI(SPI_HandleTypeDef *hspi);
void BSP_ANALOG_RegisterDriver(
    ANALOG_INPUT_DrvTypeDef *drv);

ANALOG_INPUT_StatusTypeDef BSP_ANALOG_Init(
    ANALOG_INPUT_InitTypeDef *init);

ANALOG_INPUT_StatusTypeDef BSP_ANALOG_Read(
    uint16_t *data);

ANALOG_INPUT_StatusTypeDef BSP_ANALOG_GetStatus(
    uint8_t *status);

#ifdef __cplusplus
}
#endif

#endif
