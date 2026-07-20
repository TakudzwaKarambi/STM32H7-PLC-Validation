
#include "analog_input.h"


static ANALOG_INPUT_DrvTypeDef *AnalogDrv;



void BSP_ANALOG_RegisterDriver(
    ANALOG_INPUT_DrvTypeDef *drv)
{
    AnalogDrv = drv;
}



ANALOG_INPUT_StatusTypeDef BSP_ANALOG_Init(
    ANALOG_INPUT_InitTypeDef *init)
{
    if(AnalogDrv == 0)
    {
        return ANALOG_INPUT_ERROR;
    }

    return AnalogDrv->Init(init);
}



ANALOG_INPUT_StatusTypeDef BSP_ANALOG_Read(
    uint16_t *data)
{
    if(AnalogDrv == 0)
    {
        return ANALOG_INPUT_ERROR;
    }

    return AnalogDrv->ReadRaw(data);
}


ANALOG_INPUT_StatusTypeDef BSP_ANALOG_GetStatus(
    uint8_t *status)
{
    if(AnalogDrv == 0)
    {
        return ANALOG_INPUT_ERROR;
    }

    return AnalogDrv->GetStatus(status);
}
