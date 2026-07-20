#include "analog_output.h"
#include <stddef.h>
static ANALOG_OUTPUT_DrvTypeDef *AnalogOutDrv = NULL;

void BSP_ANALOG_OUTPUT_RegisterDriver(
    ANALOG_OUTPUT_DrvTypeDef *drv)
{
    AnalogOutDrv = drv;
}

ANALOG_OUTPUT_StatusTypeDef
BSP_ANALOG_OUTPUT_Init(
    ANALOG_OUTPUT_InitTypeDef *init)
{
    if(AnalogOutDrv == NULL)
    {
        return ANALOG_OUTPUT_ERROR;
    }

    return AnalogOutDrv->Init(init);
}

ANALOG_OUTPUT_StatusTypeDef
BSP_ANALOG_OUTPUT_Write(
    uint16_t *data)
{
    if(AnalogOutDrv == NULL)
    {
        return ANALOG_OUTPUT_ERROR;
    }

    return AnalogOutDrv->WriteRaw(data);
}

ANALOG_OUTPUT_StatusTypeDef
BSP_ANALOG_OUTPUT_GetStatus(
    uint8_t *status)
{
    if(AnalogOutDrv == NULL)
    {
        return ANALOG_OUTPUT_ERROR;
    }

    return AnalogOutDrv->GetStatus(status);
}
