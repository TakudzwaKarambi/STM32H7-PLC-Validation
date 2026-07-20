#ifndef ANALOG_OUTPUT_H
#define ANALOG_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
    ANALOG_OUTPUT_OK = 0,
    ANALOG_OUTPUT_ERROR

} ANALOG_OUTPUT_StatusTypeDef;

typedef struct
{
    uint32_t dummy;
}
ANALOG_OUTPUT_InitTypeDef;

typedef struct
{
    ANALOG_OUTPUT_StatusTypeDef
        (*Init)(
            ANALOG_OUTPUT_InitTypeDef *init);

    void
        (*DeInit)(void);

    ANALOG_OUTPUT_StatusTypeDef
        (*WriteRaw)(
            uint16_t *data);

    ANALOG_OUTPUT_StatusTypeDef
        (*GetStatus)(
            uint8_t *status);

}
ANALOG_OUTPUT_DrvTypeDef;

void BSP_ANALOG_OUTPUT_RegisterDriver(
    ANALOG_OUTPUT_DrvTypeDef *drv);

ANALOG_OUTPUT_StatusTypeDef
BSP_ANALOG_OUTPUT_Init(
    ANALOG_OUTPUT_InitTypeDef *init);

ANALOG_OUTPUT_StatusTypeDef
BSP_ANALOG_OUTPUT_Write(
    uint16_t *data);

ANALOG_OUTPUT_StatusTypeDef
BSP_ANALOG_OUTPUT_GetStatus(
    uint8_t *status);

#ifdef __cplusplus
}
#endif

#endif
