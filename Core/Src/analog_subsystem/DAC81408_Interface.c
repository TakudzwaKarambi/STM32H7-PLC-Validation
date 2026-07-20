#include "DAC81408_Interface.h"

static DAC81408_t *g_dacHandle = NULL;

/* Private Functions */
static ANALOG_OUTPUT_StatusTypeDef
DAC81408_Interface_Init(
    ANALOG_OUTPUT_InitTypeDef *init);

static void
DAC81408_Interface_DeInit(void);

static ANALOG_OUTPUT_StatusTypeDef
DAC81408_Interface_WriteRaw(
    uint16_t *data);

static ANALOG_OUTPUT_StatusTypeDef
DAC81408_Interface_GetStatus(
    uint8_t *status);

/* Driver Object */
ANALOG_OUTPUT_DrvTypeDef DAC81408Drv =
{
    DAC81408_Interface_Init,
    DAC81408_Interface_DeInit,
    DAC81408_Interface_WriteRaw,
    DAC81408_Interface_GetStatus
};

/* Registration */
void DAC81408_RegisterDevice(
    DAC81408_t *dac)
{
    g_dacHandle = dac;
}

/* Init */
static ANALOG_OUTPUT_StatusTypeDef
DAC81408_Interface_Init(
    ANALOG_OUTPUT_InitTypeDef *init)
{
    (void)init;

    if(g_dacHandle == NULL)
    {
        return ANALOG_OUTPUT_ERROR;
    }

    return ANALOG_OUTPUT_OK;
}

/* DeInit */
static void
DAC81408_Interface_DeInit(void)
{
}

/* Write Raw Outputs */
static ANALOG_OUTPUT_StatusTypeDef
DAC81408_Interface_WriteRaw(
    uint16_t *data)
{
    if(g_dacHandle == NULL)
    {
        return ANALOG_OUTPUT_ERROR;
    }

    if(data == NULL)
    {
        return ANALOG_OUTPUT_ERROR;
    }

    if(DAC81408_WriteAll(
            g_dacHandle,
            data) != HAL_OK)
    {
        return ANALOG_OUTPUT_ERROR;
    }

    return ANALOG_OUTPUT_OK;
}

/* Status */
static ANALOG_OUTPUT_StatusTypeDef
DAC81408_Interface_GetStatus(
    uint8_t *status)
{
    const DAC81408_Status_t *dacStatus;

    if(status == NULL)
    {
        return ANALOG_OUTPUT_ERROR;
    }

    dacStatus =
        DAC81408_GetStatus(
            g_dacHandle);

    *status = 0;

    if(dacStatus->tempAlarm)
    {
        *status |= (1U << 0);
    }

    if(dacStatus->crcAlarm)
    {
        *status |= (1U << 1);
    }

    if(!dacStatus->spiHealthy)
    {
        *status |= (1U << 2);
    }

    return ANALOG_OUTPUT_OK;
}
