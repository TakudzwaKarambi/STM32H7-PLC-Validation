#include "dac81408.h"
#include "spi.h"
#include <string.h>

HAL_StatusTypeDef DAC81408_Init(
    DAC81408_t *dac,
    SPI_HandleTypeDef *spiHandle)
{
    if((dac == NULL) || (spiHandle == NULL))
    {
        return HAL_ERROR;
    }

    dac->spiHandle = spiHandle;

    return HAL_OK;
}
static inline void DAC_CS_LOW(DAC81408_t *dac)
{
    HAL_GPIO_WritePin(
        dac->csPort,
        dac->csPin,
        GPIO_PIN_RESET);
}

static inline void DAC_CS_HIGH(DAC81408_t *dac)
{
    HAL_GPIO_WritePin(
        dac->csPort,
        dac->csPin,
        GPIO_PIN_SET);
}

HAL_StatusTypeDef DAC81408_Reset(
    DAC81408_t *dac)
{
    HAL_GPIO_WritePin(
        dac->resetPort,
        dac->resetPin,
        GPIO_PIN_RESET);

    HAL_Delay(10);

    HAL_GPIO_WritePin(
        dac->resetPort,
        dac->resetPin,
        GPIO_PIN_SET);

    HAL_Delay(10);

    return HAL_OK;
}


HAL_StatusTypeDef DAC81408_WriteRegister(
    DAC81408_t *dac,
    uint8_t reg,
    uint16_t data)
{
    uint8_t tx[3];

    tx[0] = (reg & 0x7F);
    tx[1] = (data >> 8);
    tx[2] = (data & 0xFF);

    DAC_CS_LOW(dac);

    HAL_StatusTypeDef ret =
        HAL_SPI_Transmit(
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
    uint8_t tx[3];
    uint8_t rx[3];

    tx[0] = 0x80 | (reg & 0x7F);
    tx[1] = 0x00;
    tx[2] = 0x00;

    DAC_CS_LOW(dac);

    HAL_SPI_Transmit(
        dac->spiHandle,
        tx,
        3,
        100);

    DAC_CS_HIGH(dac);

    DAC_CS_LOW(dac);

    memset(tx,0,sizeof(tx));

    HAL_StatusTypeDef ret =
        HAL_SPI_TransmitReceive(
            dac->spiHandle,
            tx,
            rx,
            3,
            100);

    DAC_CS_HIGH(dac);

    *data =
        ((uint16_t)rx[1] << 8)
        | rx[2];

    return ret;
}

HAL_StatusTypeDef DAC81408_UpdateOutputs(
    DAC81408_t *dac)
{
    HAL_GPIO_WritePin(
        dac->ldacPort,
        dac->ldacPin,
        GPIO_PIN_RESET);

    HAL_Delay(1);

    HAL_GPIO_WritePin(
        dac->ldacPort,
        dac->ldacPin,
        GPIO_PIN_SET);

    return HAL_OK;
}


HAL_StatusTypeDef DAC81408_WriteAll(
    DAC81408_t *dac,
    uint16_t values[8])
{
    for(uint8_t i=0;i<8;i++)
    {
        if(DAC81408_WriteChannel(
                dac,
                i,
                values[i]) != HAL_OK)
        {
            return HAL_ERROR;
        }
    }

    return DAC81408_UpdateOutputs(dac);
}

