#include "output_driver.h"
#include <stddef.h>
#include "iso8200aq.h"
#include "iso8200aq_target_config.h"

#define OUTPUT_DEVICE_ID    (0U)

static uint8_t g_outputBitmap = 0;


/*------------------------------------------------------------------*/
/* Initialization                                                   */
/*------------------------------------------------------------------*/

void OutputDriver_Init(void)
{
    iso8200aq_Init_t init;

    init.spiFreq =
        ISO8200AQ_CONF_PARAM_SPI_FREQ;

    init.timingTcss =
        ISO8200AQ_CONF_PARAM_TIMING_TCSS;

    init.chanEnBitmap =
        ISO8200AQ_CONF_PARAM_CHANNEL_ENABLE_BITMAP_DEVICE_0;

    Iso8200aq_Init(&init);

    Iso8200aq_SetNbDevices(1);

    Iso8200aq_SetOutEnable(
        OUTPUT_DEVICE_ID,
        1);

    g_outputBitmap =
        init.chanEnBitmap;
}

bool OutputDriver_Write(
    uint8_t bitmap)
{
    if(!Iso8200aq_SetAllChannelStatus(
            OUTPUT_DEVICE_ID,
            bitmap))
    {
        return false;
    }

    g_outputBitmap = bitmap;

    return true;
}

bool OutputDriver_SetChannel(
    uint8_t channel,
    bool state)
{
    if(channel >= 8)
        return false;

    if(!Iso8200aq_SetOneChannelStatus(
            OUTPUT_DEVICE_ID,
            channel,
            state ? 1 : 0))
    {
        return false;
    }

    if(state)
    {
        g_outputBitmap |=
            (1U << channel);
    }
    else
    {
        g_outputBitmap &=
            ~(1U << channel);
    }

    return true;
}

uint8_t OutputDriver_GetOutputs(void)
{
    return g_outputBitmap;
}


bool OutputDriver_ReadStatus(
    OutputDriver_Status_t *status)
{
    if(status == NULL)
        return false;

    status->OutputBitmap =
        Iso8200aq_GetAllChannelStatus(
            OUTPUT_DEVICE_ID);

    status->FaultRegister =
        Iso8200aq_GetFaultRegister(
            OUTPUT_DEVICE_ID);

    status->PowerGood =
        Iso8200aq_GetPGoodStatus(
            OUTPUT_DEVICE_ID) ? true : false;

    status->Fault =
        Iso8200aq_GetFaultStatus(
            OUTPUT_DEVICE_ID) ? true : false;

    status->OutputEnabled =
        Iso8200aq_GetOutEnable(
            OUTPUT_DEVICE_ID) ? true : false;

    return true;
}

bool OutputDriver_ClearFaults(void)
{
    return Iso8200aq_SetAllChannelStatus(
        OUTPUT_DEVICE_ID,
        g_outputBitmap);
}

bool OutputDriver_Enable(void)
{
    return Iso8200aq_SetOutEnable(
        OUTPUT_DEVICE_ID,
        1);
}

bool OutputDriver_IsEnabled(void)
{
    return Iso8200aq_GetOutEnable(
        OUTPUT_DEVICE_ID)
        ? true : false;
}
