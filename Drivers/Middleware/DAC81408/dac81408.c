#include "dac81408.h"
#include <string.h>

/* Private CS Control Helpers */
static inline void DAC_CS_LOW(DAC81408_t *dac)
{
    if (dac != NULL && dac->csPort != NULL)
    {
        HAL_GPIO_WritePin(dac->csPort, dac->csPin, GPIO_PIN_RESET);
    }
}

static inline void DAC_CS_HIGH(DAC81408_t *dac)
{
    if (dac != NULL && dac->csPort != NULL)
    {
        HAL_GPIO_WritePin(dac->csPort, dac->csPin, GPIO_PIN_SET);
    }
}

/* API Implementation */

HAL_StatusTypeDef DAC81408_Init(
    DAC81408_t *dac,
    SPI_HandleTypeDef *spiHandle)
{
    if ((dac == NULL) || (spiHandle == NULL))
    {
        return HAL_ERROR;
    }

    dac->spiHandle = spiHandle;

    return HAL_OK;
}

HAL_StatusTypeDef DAC81408_Reset(
    DAC81408_t *dac)
{
    if (dac == NULL) return HAL_ERROR;

    if (dac->resetPort != NULL)
    {
        HAL_GPIO_WritePin(dac->resetPort, dac->resetPin, GPIO_PIN_RESET);
        HAL_Delay(10);
        HAL_GPIO_WritePin(dac->resetPort, dac->resetPin, GPIO_PIN_SET);
        HAL_Delay(10);
    }

    return HAL_OK;
}

HAL_StatusTypeDef DAC81408_WriteRegister(
    DAC81408_t *dac,
    uint8_t reg,
    uint16_t data)
{
    if ((dac == NULL) || (dac->spiHandle == NULL)) return HAL_ERROR;

    uint8_t tx[3];

    tx[0] = (reg & 0x7F);        // Bit 7 = 0 for Write
    tx[1] = (uint8_t)(data >> 8);
    tx[2] = (uint8_t)(data & 0xFF);

    DAC_CS_LOW(dac);

    HAL_StatusTypeDef ret = HAL_SPI_Transmit(
        dac->spiHandle,
        tx,
        3,
        100);

    DAC_CS_HIGH(dac);

    return ret;
}

HAL_StatusTypeDef DAC81408_ReadRegister(
    DAC81408_t *dac,
    uint8_t reg,
    uint16_t *data)
{
    if ((dac == NULL) || (dac->spiHandle == NULL) || (data == NULL))
    {
        return HAL_ERROR;
    }

    uint8_t tx[3];
    uint8_t rx[3];

    /* Frame 1: Request register read */
    tx[0] = 0x80 | (reg & 0x7F);  // Bit 7 = 1 for Read
    tx[1] = 0x00;
    tx[2] = 0x00;

    DAC_CS_LOW(dac);
    HAL_SPI_Transmit(dac->spiHandle, tx, 3, 100);
    DAC_CS_HIGH(dac);

    /* Frame 2: Clock out data using dummy bytes */
    DAC_CS_LOW(dac);
    memset(tx, 0, sizeof(tx));

    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(
        dac->spiHandle,
        tx,
        rx,
        3,
        100);

    DAC_CS_HIGH(dac);

    *data = ((uint16_t)rx[1] << 8) | rx[2];

    return ret;
}

HAL_StatusTypeDef DAC81408_WriteChannel(
    DAC81408_t *dac,
    uint8_t channel,
    uint16_t value)
{
    if ((dac == NULL) || (channel >= DAC81408_NUM_CHANNELS))
    {
        return HAL_ERROR;
    }

    uint8_t reg = DAC81408_REG_DAC0 + channel;
    return DAC81408_WriteRegister(dac, reg, value);
}

HAL_StatusTypeDef DAC81408_SetRange(
    DAC81408_t *dac,
    uint8_t channel,
    DAC81408_Range_t range)
{
    if ((dac == NULL) || (channel >= DAC81408_NUM_CHANNELS))
    {
        return HAL_ERROR;
    }

    uint8_t reg = (channel < 4) ? DAC81408_REG_DACRANGE0 : DAC81408_REG_DACRANGE1;
    uint8_t shift = (channel % 4) * 4;

    uint16_t currentVal = 0;
    if (DAC81408_ReadRegister(dac, reg, &currentVal) != HAL_OK)
    {
        return HAL_ERROR;
    }

    currentVal &= ~(0x0FU << shift);
    currentVal |= (((uint16_t)range & 0x0FU) << shift);

    return DAC81408_WriteRegister(dac, reg, currentVal);
}

HAL_StatusTypeDef DAC81408_UpdateOutputs(
    DAC81408_t *dac)
{
    if (dac == NULL) return HAL_ERROR;

    /* Toggle hardware LDAC only if pin is assigned */
    if (dac->ldacPort != NULL)
    {
        HAL_GPIO_WritePin(dac->ldacPort, dac->ldacPin, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(dac->ldacPort, dac->ldacPin, GPIO_PIN_SET);
    }

    return HAL_OK;
}

HAL_StatusTypeDef DAC81408_WriteAll(
    DAC81408_t *dac,
    uint16_t values[8])
{
    if ((dac == NULL) || (values == NULL)) return HAL_ERROR;

    for (uint8_t i = 0; i < DAC81408_NUM_CHANNELS; i++)
    {
        if (DAC81408_WriteChannel(dac, i, values[i]) != HAL_OK)
        {
            return HAL_ERROR;
        }
    }

    return DAC81408_UpdateOutputs(dac);
}

HAL_StatusTypeDef DAC81408_ReadStatus(
    DAC81408_t *dac)
{
    if (dac == NULL) return HAL_ERROR;

    uint16_t statusReg = 0;
    if (DAC81408_ReadRegister(dac, DAC81408_REG_STATUS, &statusReg) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Bit 3: Thermal Alarm, Bit 2: CRC Alarm, Bit 1: Busy */
    dac->status.tempAlarm = (statusReg & (1U << 3)) ? true : false;
    dac->status.crcAlarm  = (statusReg & (1U << 2)) ? true : false;
    dac->status.busy      = (statusReg & (1U << 1)) ? true : false;

    return HAL_OK;
}

const DAC81408_Status_t* DAC81408_GetStatus(
    DAC81408_t *dac)
{
    if (dac == NULL) return NULL;
    return &(dac->status);
}
/* In dac81408.c */
bool DAC81408_ValidateSPI(DAC81408_t *dac)
{
    if (dac == NULL || dac->spiHandle == NULL) return false;

    uint16_t readVal = 0;

    /* Read Device ID Register (0x01) -> Expected 0x029C */
    if (DAC81408_ReadRegister(dac, DAC81408_REG_DEVICEID, &readVal) != HAL_OK)
    {
        return false;
    }
    if (readVal != 0x029C)
    {
        return false;
    }

    /* Write/Read-Back test on General Config (0x04) */
    uint16_t testPattern = 0x0A00;
    if (DAC81408_WriteRegister(dac, DAC81408_REG_GENCONFIG, testPattern) != HAL_OK)
    {
        return false;
    }
    if (DAC81408_ReadRegister(dac, DAC81408_REG_GENCONFIG, &readVal) != HAL_OK)
    {
        return false;
    }

    return (readVal == testPattern);
}
