#include "ADS8688_Interface.h"


static ADS8688 *g_adsHandle = NULL;



static ANALOG_INPUT_StatusTypeDef
ADS8688_Interface_Init(
    ANALOG_INPUT_InitTypeDef *init);

static void
ADS8688_Interface_DeInit(void);

static ANALOG_INPUT_StatusTypeDef
ADS8688_Interface_ReadRaw(
    uint16_t *data);

static ANALOG_INPUT_StatusTypeDef
ADS8688_Interface_GetStatus(
    uint8_t *status);



ANALOG_INPUT_DrvTypeDef ADS8688Drv =
{
    ADS8688_Interface_Init,
    ADS8688_Interface_DeInit,
    ADS8688_Interface_ReadRaw,
    ADS8688_Interface_GetStatus
};



void ADS8688_RegisterDevice(
    ADS8688 *ads)
{
    g_adsHandle = ads;  //this is the registration of our spi configuration etc
}



static ANALOG_INPUT_StatusTypeDef
ADS8688_Interface_Init(
    ANALOG_INPUT_InitTypeDef *init)
{
    (void)init;

    if(g_adsHandle == NULL)
    {
        return ANALOG_INPUT_ERROR;
    }

    return ANALOG_INPUT_OK;
}



static void
ADS8688_Interface_DeInit(void)
{
    /* Nothing required currently */
}



static ANALOG_INPUT_StatusTypeDef
ADS8688_Interface_ReadRaw(
    uint16_t *data)
{
    HAL_StatusTypeDef halStatus;

    if(g_adsHandle == NULL)
    {
        return ANALOG_INPUT_ERROR;
    }

    if(data == NULL)
    {
        return ANALOG_INPUT_ERROR;
    }

    halStatus =
        ADS_Read_All_Raw(
            g_adsHandle,
            data);

    if(halStatus != HAL_OK)
    {
        return ANALOG_INPUT_ERROR;
    }

    return ANALOG_INPUT_OK;
}



static ANALOG_INPUT_StatusTypeDef
ADS8688_Interface_GetStatus(
    uint8_t *status)
{
    if(status == NULL)
    {
        return ANALOG_INPUT_ERROR;
    }

    /*
      Future:
      Alarm registers
      Communication status
      ADC diagnostics
    */

    *status = 0U;

    return ANALOG_INPUT_OK;
}
