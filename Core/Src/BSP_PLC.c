/**
  ******************************************************************************
  * @file           : BSP_PLC.c
  * @brief          : Board Support Package for Industrial PLC core processing
  ******************************************************************************
  */

#include "bsp_plc.h"
#include "process_image.h"
#include "spi.h"
#include "main.h"

/* Digital Peripherals */
#include "current_limiter.h"
#include "output_driver.h"
#include "clt01_38sq7.h"
#include "current_limiter.h"


/* Analog Inputs (ADC) */
#include "ADS8688.h"
#include "ADS8688_Interface.h"
#include "analog_input.h"

/* Analog Outputs (DAC) */
#include "dac81408.h"
#include "DAC81408_Interface.h"
#include "analog_output.h"

/*============================================================================
  PRIVATE VARIABLES & HARDWARE INSTANCES
============================================================================*/
static bool bspInitialized = false;

static ADS8688    g_ads;
static DAC81408_t g_dac;

static uint16_t adsRaw[8];
static uint16_t aoRaw[8];

/*============================================================================
  ADC CONFIGURATION REGISTER MAP
============================================================================*/
static ADS8688_Config_t g_adsConfig =
{
    .range =
    {
        0x06, // CH0 Range
        0x06, // CH1 Range
        0x05, // CH2 Range
        0x05, // CH3 Range
        0x05, // CH4 Range
        0x05, // CH5 Range
        0x06, // CH6 Range
        0x06  // CH7 Range
    }
};

/*============================================================================
  STATUS INDICATOR FUNCTIONS (USER LED)
============================================================================*/
void BSP_LED_On(void)
{
    HAL_GPIO_WritePin(USR_LED_GPIO_Port, USR_LED_Pin, GPIO_PIN_SET);
}

void BSP_LED_Off(void)
{
    HAL_GPIO_WritePin(USR_LED_GPIO_Port, USR_LED_Pin, GPIO_PIN_RESET);
}

void BSP_LED_Toggle(void)
{
    HAL_GPIO_TogglePin(USR_LED_GPIO_Port, USR_LED_Pin);
}

/*============================================================================
  HARDWARE INITIALIZATION SUBSYSTEM
============================================================================*/
bool BSP_PLC_Init(void)
{
    ANALOG_INPUT_InitTypeDef aiInit =
    {
        .channelCount = 8
    };

    /*--------------------------------------------------
      1. Digital Inputs Initialization
    --------------------------------------------------*/
    if (CurrentLimiter_Init() != HAL_OK)
    {
        SystemStatus.clt01Fault = true;
        return false;
    }
    SystemStatus.clt01Fault = false;

    /*--------------------------------------------------
      2. Digital Outputs Initialization
    --------------------------------------------------*/
    OutputDriver_Init();
    SystemStatus.iso8200Fault = false;

    /*--------------------------------------------------
      3. ADS8688 ADC Peripheral Setup
    --------------------------------------------------*/
    if (ADS8688_Init(&g_ads, &hspi4,
                     ADS_CS_GPIO_Port, ADS_CS_Pin,
                     ADS_RST_GPIO_Port, ADS_RST_Pin,
                     &g_adsConfig) != HAL_OK)
    {
        SystemStatus.ads8688Fault = true;
        return false;
    }

    ADS8688_RegisterDevice(&g_ads);
    BSP_ANALOG_RegisterDriver(&ADS8688Drv);

    if (BSP_ANALOG_Init(&aiInit) != ANALOG_INPUT_OK)
    {
        SystemStatus.ads8688Fault = true;
        return false;
    }
    SystemStatus.ads8688Fault = false;

    /*--------------------------------------------------
      4. DAC81408 DAC Peripheral Setup
    --------------------------------------------------*/
    if (DAC81408_Init(&g_dac, &hspi4) != HAL_OK)
    {
        SystemStatus.dac81408Fault = true;
        return false;
    }

    BSP_ANALOG_OUTPUT_RegisterDriver(&DAC81408Drv);

    if (BSP_ANALOG_OUTPUT_Init(NULL) != ANALOG_OUTPUT_OK)
    {
        SystemStatus.dac81408Fault = true;
        return false;
    }
    SystemStatus.dac81408Fault = false;

    /* Initialization Complete */
    bspInitialized = true;
    return true;
}

/*============================================================================
  DATA REFRESH SUBROUTINES (PROCESS IMAGE SYNCHRONIZATION)
============================================================================*/

void BSP_PLC_UpdateInputs(void)
{
    CLT01_Status_t clt;

    // 1. Check if the SPI hardware transmission succeeded
    if (CurrentLimiter_Read(&clt) != HAL_OK)
    {
        SystemStatus.clt01Fault = true;
        return;
    }

    // 2. NEW CRITICAL CHECK: Check if the chip is alive and the data frame passed parity/stop checks
    if (!CLT01_IsAlive(&clt))
    {
        SystemStatus.clt01Fault = true; // Flag a fault if reading garbage data
        return;
    }

    SystemStatus.clt01Fault = false;

    /* Map hardware bits to the PLC process image */
    InputImage.digital.LEH  = ((clt.Inputs >> 0) & 1U);
    InputImage.digital.LEL  = ((clt.Inputs >> 1) & 1U);
    InputImage.digital.AS01 = ((clt.Inputs >> 2) & 1U);
    InputImage.digital.AS02 = ((clt.Inputs >> 3) & 1U);
}

void BSP_PLC_UpdateAnalogInputs(void)
{
    if (BSP_ANALOG_Read(adsRaw) != ANALOG_INPUT_OK)
    {
        SystemStatus.ads8688Fault = true;
        return;
    }
    SystemStatus.ads8688Fault = false;

    /* Map parsed voltages to process space channels */
    InputImage.analog.PT01 = (float)adsRaw[0];
    InputImage.analog.PT02 = (float)adsRaw[1];
    InputImage.analog.PT03 = (float)adsRaw[2];
    InputImage.analog.FM01 = (float)adsRaw[3];
    InputImage.analog.FM02 = (float)adsRaw[4];
}

void BSP_PLC_UpdateOutputs(void)
{
    uint8_t outputs = 0U;

    if (OutputImage.digital.SA1) outputs |= (1U << 0);
    if (OutputImage.digital.SA2) outputs |= (1U << 1);
    if (OutputImage.digital.SA3) outputs |= (1U << 2);
    if (OutputImage.digital.SA4) outputs |= (1U << 3);

    OutputDriver_Write(outputs);
}

void BSP_PLC_UpdateAnalogOutputs(void)
{
    aoRaw[0] = OutputImage.analog.AO01;
    aoRaw[1] = OutputImage.analog.AO02;
    aoRaw[2] = OutputImage.analog.AO03;
    aoRaw[3] = OutputImage.analog.AO04;
    aoRaw[4] = OutputImage.analog.AO05;
    aoRaw[5] = OutputImage.analog.AO06;
    aoRaw[6] = OutputImage.analog.AO07;
    aoRaw[7] = OutputImage.analog.AO08;

    BSP_ANALOG_OUTPUT_Write(aoRaw);
}

/*============================================================================
  DIAGNOSTICS & SYSTEM HEALTH STATUS MONITORING
============================================================================*/
void BSP_PLC_UpdateDiagnostics(void)
{
    uint8_t aiStatus = 0;
    uint8_t aoStatus = 0;

    if (BSP_ANALOG_GetStatus(&aiStatus) != ANALOG_INPUT_OK)
    {
        SystemStatus.ads8688Fault = true;
    }

    if (BSP_ANALOG_OUTPUT_GetStatus(&aoStatus) != ANALOG_OUTPUT_OK)
    {
        SystemStatus.dac81408Fault = true;
    }

    /* Aggregate System Health Assessment */
    SystemStatus.systemHealthy = true;

    if (SystemStatus.clt01Fault)     SystemStatus.systemHealthy = false;
    if (SystemStatus.iso8200Fault)   SystemStatus.systemHealthy = false;
    if (SystemStatus.ads8688Fault)   SystemStatus.systemHealthy = false;
    if (SystemStatus.dac81408Fault)  SystemStatus.systemHealthy = false;
}

bool BSP_PLC_IsInitialized(void)
{
    return bspInitialized;
}
