/******************************************************************************
 * @file    clt01_38sq7.c
 * @brief   Driver for STMicroelectronics CLT01-38SQ7
 *
 * This driver supports the CLT01-38SQ7 8-channel industrial digital input
 * interface using the STM32 HAL SPI interface.
 *
 * The driver is organized into:
 *   - Low-level SPI communication
 *   - Frame validation
 *   - Frame decoding
 *   - Public API
 *
 ******************************************************************************/

#include "clt01_38sq7.h"
#include "clt01_38sq7_cfg.h"
#include "clt01_38sq7_private.h"

#include <string.h>


/******************************************************************************
 * Local Helper Functions
 ******************************************************************************/
/******************************************************************************
 * Local Helper Functions
 ******************************************************************************/

/**
 * @brief Returns the logic state of one CLT input.
 *
 * Channel numbering:
 *      0 -> IN1
 *      1 -> IN2
 *      ...
 *      7 -> IN8
 */
static inline bool CLT_GetInputBit(uint16_t frame, uint8_t channel)
{
    return ((frame >> (CLT_INPUT_SHIFT + channel)) & 0x01U);
}
/**
 * @brief Assert Chip Select.
 */
void CLT_CS_Low(CLT01_Handle_t *hclt)
{
    HAL_GPIO_WritePin(
        hclt->CS_Port,
        hclt->CS_Pin,
        GPIO_PIN_RESET);
}


/**
 * @brief Deassert Chip Select.
 */
void CLT_CS_High(CLT01_Handle_t *hclt)
{
    HAL_GPIO_WritePin(
        hclt->CS_Port,
        hclt->CS_Pin,
        GPIO_PIN_SET);
}


/******************************************************************************
 * SPI Communication
 ******************************************************************************/

/**
 * @brief Read one complete SPI frame.
 *
 * Performs one SPI transaction and returns the received
 * 16-bit CLT status frame.
 *
 * @param hclt
 * @param frame
 *
 * @retval HAL Status
 */
HAL_StatusTypeDef CLT_SPI_ReadFrame(
        CLT01_Handle_t *hclt,
        uint16_t *frame)
{
#if CLT01_ENABLE_PARAMETER_CHECKS

    if ((hclt == NULL) || (frame == NULL))
    {
        return HAL_ERROR;
    }

#endif

#if CLT01_SPI_16BIT_MODE

    uint8_t tx[2] = {0x00U, 0x00U};
    uint8_t rx[2] = {0x00U, 0x00U};

    CLT_CS_Low(hclt);

    HAL_StatusTypeDef status =
        HAL_SPI_TransmitReceive(
            hclt->hspi,
            tx,
            rx,
            2U,
            CLT01_SPI_TIMEOUT_MS);

    CLT_CS_High(hclt);

    if (status != HAL_OK)
    {
        return status;
    }

    *frame =
        ((uint16_t)rx[0] << 8U) |
        ((uint16_t)rx[1]);

#else

#error "8-bit SPM mode not yet implemented."

#endif

    return HAL_OK;
}


/******************************************************************************
 * Frame Validation
 ******************************************************************************/

/**
 * @brief Validate parity bits.
 *
 * Datasheet equations:
 *
 * PC1 = XNOR(IN1, IN4)
 * PC2 = XNOR(IN2, IN3)
 * PC3 = XNOR(IN5, IN6)
 * PC4 = XNOR(IN7, IN8)
 *
 * @retval true  Frame parity valid
 * @retval false Frame parity invalid
 */
bool CLT_CheckParity(uint16_t frame)
{
    bool in1 = (frame >> 8U)  & 0x01U;
    bool in2 = (frame >> 9U)  & 0x01U;
    bool in3 = (frame >> 10U) & 0x01U;
    bool in4 = (frame >> 11U) & 0x01U;
    bool in5 = (frame >> 12U) & 0x01U;
    bool in6 = (frame >> 13U) & 0x01U;
    bool in7 = (frame >> 14U) & 0x01U;
    bool in8 = (frame >> 15U) & 0x01U;

    bool pc1 = (frame & CLT_PC1_MASK) != 0U;
    bool pc2 = (frame & CLT_PC2_MASK) != 0U;
    bool pc3 = (frame & CLT_PC3_MASK) != 0U;
    bool pc4 = (frame & CLT_PC4_MASK) != 0U;

    bool calc1 = !(in1 ^ in4);
    bool calc2 = !(in2 ^ in3);
    bool calc3 = !(in5 ^ in6);
    bool calc4 = !(in7 ^ in8);

    return
        (pc1 == calc1) &&
        (pc2 == calc2) &&
        (pc3 == calc3) &&
        (pc4 == calc4);
}


/**
 * @brief Validate stop bits.
 *
 * Valid CLT frame:
 *
 * STOP_LOW  = 0
 * STOP_HIGH = 1
 *
 * Binary:
 *
 * Bit1 = 0
 * Bit0 = 1
 *
 * @retval true  Stop bits valid
 * @retval false Invalid frame
 */
bool CLT_CheckStopBits(uint16_t frame)
{
    bool stopLow =
        (frame & CLT_STOP_LOW_MASK) != 0U;

    bool stopHigh =
        (frame & CLT_STOP_HIGH_MASK) != 0U;

    return
        (!stopLow) &&
        (stopHigh);
}

/******************************************************************************
 * Frame Decoder
 ******************************************************************************/

/**
 * @brief Decode the eight digital input channels.
 */
void CLT_DecodeInputs(
        uint16_t frame,
        CLT01_Status_t *status)
{
    uint8_t i;

    status->Inputs =
        (uint8_t)((frame & CLT_INPUT_MASK) >> CLT_INPUT_SHIFT);

#if CLT01_DECODE_CHANNELS

    for(i = 0U; i < 8U; i++)
    {
        status->Channel[i] =
            CLT_GetInputBit(frame, i);
    }

#endif
}


/**
 * @brief Decode device diagnostic bits.
 */
void CLT_DecodeDiagnostics(
        uint16_t frame,
        CLT01_Status_t *status)
{
    /*
     * Active LOW+
     */

    status->SupplyVoltageAlarm =
        ((frame & CLT_UVA_MASK) == 0U);

    status->OverTemperatureAlarm =
        ((frame & CLT_OTA_MASK) == 0U);
}

/******************************************************************************
 * Hardware Diagnostics
 ******************************************************************************/

/**
 * @brief Read optional hardware diagnostic pins.
 */
void CLT_UpdateHardwareStatus(
        CLT01_Handle_t *hclt,
        CLT01_Status_t *status)
{

#if CLT01_ENABLE_GOOD_PIN

    if(hclt->GOOD_Port != NULL)
    {
        status->GoodPinState =
            (HAL_GPIO_ReadPin(
                hclt->GOOD_Port,
                hclt->GOOD_Pin) == GPIO_PIN_SET);
    }
    else
    {
        status->GoodPinState = true;
    }

#else

    status->GoodPinState = true;

#endif



#if CLT01_ENABLE_FAULT_PIN

    if(hclt->FAULT_Port != NULL)
    {
        status->FaultPinState =
            (HAL_GPIO_ReadPin(
                hclt->FAULT_Port,
                hclt->FAULT_Pin) == GPIO_PIN_SET);
    }
    else
    {
        status->FaultPinState = false;
    }

#else

    status->FaultPinState = false;

#endif
}


/******************************************************************************
 * Frame Validation
 ******************************************************************************/

/**
 * @brief Update frame integrity and device status.
 */
void CLT_UpdateFrameStatus(
        CLT01_Status_t *status)
{

#if CLT01_ENABLE_PARITY_CHECK

    status->ParityValid =
        CLT_CheckParity(status->RawFrame);

#else

    status->ParityValid = true;

#endif



#if CLT01_ENABLE_STOP_BIT_CHECK

    status->StopBitsValid =
        CLT_CheckStopBits(status->RawFrame);

#else

    status->StopBitsValid = true;

#endif



    status->FrameValid =
        status->ParityValid &&
        status->StopBitsValid;



    status->DeviceAlive =
            (status->RawFrame != 0x0000U) &&
            (status->RawFrame != 0xFFFFU) &&
            status->FrameValid &&
            status->GoodPinState &&
           !status->FaultPinState;
}

/******************************************************************************
 * Public Driver Functions
 ******************************************************************************/

/**
 * @brief Initialize the CLT01 device.
 *
 * Initializes the driver and enables the CLT01 digital input interface.
 *
 * @param hclt Pointer to CLT handle.
 *
 * @retval HAL_OK
 * @retval HAL_ERROR
 */
HAL_StatusTypeDef CLT01_Init(CLT01_Handle_t *hclt)
{
#if CLT01_ENABLE_PARAMETER_CHECKS

    if (hclt == NULL)
    {
        return HAL_ERROR;
    }

#endif

    /* Ensure the device is deselected */
    CLT_CS_High(hclt);

    /* Enable the CLT device if an enable pin is provided */
    if (hclt->EN_Port != NULL)
    {
        HAL_GPIO_WritePin(
                hclt->EN_Port,
                hclt->EN_Pin,
                GPIO_PIN_SET);
    }

    /* Allow the device to power up */
    HAL_Delay(CLT01_POWERUP_DELAY_MS);

    return HAL_OK;
}


/**
 * @brief Read and decode one CLT01 status frame.
 *
 * @param hclt
 * @param status
 *
 * @retval HAL Status
 */
HAL_StatusTypeDef CLT01_ReadStatus(
        CLT01_Handle_t *hclt,
        CLT01_Status_t *status)
{

#if CLT01_ENABLE_PARAMETER_CHECKS

    if ((hclt == NULL) || (status == NULL))
    {
        return HAL_ERROR;
    }

#endif


#if CLT01_CLEAR_STATUS_BEFORE_READ

    memset(status, 0, sizeof(CLT01_Status_t));

#endif


    HAL_StatusTypeDef ret =
            CLT_SPI_ReadFrame(
                    hclt,
                    &status->RawFrame);

    if(ret != HAL_OK)
    {
        return ret;
    }


    /* Decode received frame */

    CLT_DecodeInputs(
            status->RawFrame,
            status);

    CLT_DecodeDiagnostics(
            status->RawFrame,
            status);

    CLT_UpdateHardwareStatus(
            hclt,
            status);

    CLT_UpdateFrameStatus(
            status);

    return HAL_OK;
}


/**
 * @brief Perform a raw SPI communication test.
 *
 * Reads one raw CLT frame without decoding.
 *
 * Useful during board bring-up and hardware validation.
 *
 * @param hclt
 * @param rawFrame
 *
 * @retval HAL Status
 */
HAL_StatusTypeDef CLT01_TestCommunication(
        CLT01_Handle_t *hclt,
        uint16_t *rawFrame)
{

#if CLT01_ENABLE_PARAMETER_CHECKS

    if ((hclt == NULL) || (rawFrame == NULL))
    {
        return HAL_ERROR;
    }

#endif

    return CLT_SPI_ReadFrame(
            hclt,
            rawFrame);
}


/**
 * @brief Returns overall device health.
 *
 * @param status
 *
 * @retval true  Device operating correctly.
 * @retval false Device communication or diagnostics failed.
 */
bool CLT01_IsAlive(
        const CLT01_Status_t *status)
{

#if CLT01_ENABLE_PARAMETER_CHECKS

    if(status == NULL)
    {
        return false;
    }

#endif

    return status->DeviceAlive;
}
