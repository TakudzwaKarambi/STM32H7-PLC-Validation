#include "DAC81408_Interface.h"
#include <string.h>

extern SPI_HandleTypeDef hspi4;

#define DAC_CS_PORT       GPIOE
#define DAC_CS_PIN        GPIO_PIN_4

/* =========================================================================
 * HARDWARE MISHAP WORKAROUND CONFIGURATION
 * - RESET GPIO disabled: Pin 7 (A6) of Si8661 is an output pin.
 * - SDO Readback disabled: Pin 15 (B1) of Si8661 is an output pin.
 * ========================================================================= */
#define DAC_RST_PORT      NULL
#define DAC_RST_PIN       0

/* Internal Device Instance */
static DAC81408_t g_dacInstance;

/* Device Registration Definition */
void DAC81408_RegisterDevice(DAC81408_t *dac)
{
    if (dac != NULL && dac != &g_dacInstance)
    {
        memcpy(&g_dacInstance, dac, sizeof(DAC81408_t));
    }
}

/* Interface Callbacks */
static ANALOG_OUTPUT_StatusTypeDef DAC81408_Interface_Init(ANALOG_OUTPUT_InitTypeDef *init)
{
    (void)init;

    memset(&g_dacInstance, 0, sizeof(DAC81408_t));

    g_dacInstance.csPort    = DAC_CS_PORT;
    g_dacInstance.csPin     = DAC_CS_PIN;

    /* WORKAROUND 1: Set Reset GPIO to NULL to prevent output driver contention */
    g_dacInstance.resetPort = NULL;
    g_dacInstance.resetPin  = 0;

    g_dacInstance.ldacPort  = NULL;
    g_dacInstance.clrPort   = NULL;
    g_dacInstance.alarmPort = NULL;

    /* Ensure Chip Select starts HIGH (Inactive) */
    HAL_GPIO_WritePin(g_dacInstance.csPort, g_dacInstance.csPin, GPIO_PIN_SET);

    /* Initialize low-level HAL driver structure */
    if (DAC81408_Init(&g_dacInstance, &hspi4) != HAL_OK)
    {
        return ANALOG_OUTPUT_ERROR;
    }

    /* ---------------------------------------------------------------------
     * WORKAROUND 2: Hardware Reset Skipped
     * ---------------------------------------------------------------------
     * DAC81408_Reset(&g_dacInstance); // Disabled due to reverse isolator pin
     */

    /* ---------------------------------------------------------------------
     * WORKAROUND 3: SPI Readback Validation Skipped
     * ---------------------------------------------------------------------
     * SDO cannot send data back through the Si8661 on Channel 1.
     * We bypass DAC81408_ValidateSPI() to operate in Write-Only Mode.
     * --------------------------------------------------------------------- */
    /*
    if (!DAC81408_ValidateSPI(&g_dacInstance))
    {
        g_dacInstance.status.spiHealthy = false;
        return ANALOG_OUTPUT_ERROR;
    }
    */

    /* Manually force flags to allow write operations */
    g_dacInstance.status.spiHealthy  = true;
    g_dacInstance.status.initialized = true;

    return ANALOG_OUTPUT_OK;
}

static void DAC81408_Interface_DeInit(void)
{
    g_dacInstance.status.initialized = false;
    g_dacInstance.status.spiHealthy  = false;
}

static ANALOG_OUTPUT_StatusTypeDef DAC81408_Interface_WriteRaw(uint16_t *data)
{
    if (data == NULL || !g_dacInstance.status.initialized)
    {
        return ANALOG_OUTPUT_ERROR;
    }

    /* Transmit raw DAC register data via SPI (Write-Only) */
    if (DAC81408_WriteAll(&g_dacInstance, data) != HAL_OK)
    {
        return ANALOG_OUTPUT_ERROR;
    }

    return ANALOG_OUTPUT_OK;
}

static ANALOG_OUTPUT_StatusTypeDef DAC81408_Interface_GetStatus(uint8_t *status)
{
    if (status == NULL) return ANALOG_OUTPUT_ERROR;

    *status = 0;

    /* NOTE: Register readbacks fail due to SDO hardware flaw.
     * Status reports cached software flags only. */
    if (g_dacInstance.status.tempAlarm)   *status |= (1U << 0);
    if (g_dacInstance.status.crcAlarm)    *status |= (1U << 1);
    if (!g_dacInstance.status.spiHealthy) *status |= (1U << 2);

    return ANALOG_OUTPUT_OK;
}

/* Driver Interface Structure */
ANALOG_OUTPUT_DrvTypeDef DAC81408Drv =
{
    DAC81408_Interface_Init,
    DAC81408_Interface_DeInit,
    DAC81408_Interface_WriteRaw,
    DAC81408_Interface_GetStatus
};
