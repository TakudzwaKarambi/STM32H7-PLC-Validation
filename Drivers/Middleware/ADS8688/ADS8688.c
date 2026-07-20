#include "ADS8688.h"
#include <string.h>
/*
 * INITIALISATION
 */

 HAL_StatusTypeDef ADS_SPI_Transfer(
    ADS8688 *ads,
    uint8_t *tx,
    uint8_t *rx,
    uint16_t len)
{
    HAL_GPIO_WritePin(
        ads->csPinBank,
        ads->csPin,
        GPIO_PIN_RESET);

    HAL_StatusTypeDef ret =
        HAL_SPI_TransmitReceive(
            ads->spiHandle,
            tx,
            rx,
            len,
            100);

    HAL_GPIO_WritePin(
        ads->csPinBank,
        ads->csPin,
        GPIO_PIN_SET);

    if(ret != HAL_OK)
    {
        ads->status.spiHealthy = false;
        ads->status.commErrors++;
    }

    return ret;
}

HAL_StatusTypeDef ADS8688_Reset(
    ADS8688 *ads)
{
    if(ads == NULL)
    {
        return HAL_ERROR;
    }

    HAL_GPIO_WritePin(
        ads->rstPinBank,
        ads->rstPin,
        GPIO_PIN_RESET);

    HAL_Delay(10);

    HAL_GPIO_WritePin(
        ads->rstPinBank,
        ads->rstPin,
        GPIO_PIN_SET);

    HAL_Delay(20);

    return HAL_OK;
}

HAL_StatusTypeDef ADS8688_SelfTest(
    ADS8688 *ads)
{
    uint8_t reg[2];

    if(ADS_Prog_Read(
        ads,
        FEATURE_SELECT,
        reg) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}


HAL_StatusTypeDef ADS8688_Init(
    ADS8688 *ads,
    SPI_HandleTypeDef *spiHandle,
    GPIO_TypeDef *csPinBank,
    uint16_t csPin,
    GPIO_TypeDef *rstPinBank,
    uint16_t rstPin,
    const ADS8688_Config_t *config)
{
    if((ads == NULL) ||
       (spiHandle == NULL) ||
       (config == NULL))
    {
        return HAL_ERROR;
    }

    memset(ads,0,sizeof(ADS8688));

    ads->spiHandle  = spiHandle;

    ads->csPinBank  = csPinBank;
    ads->csPin      = csPin;

    ads->rstPinBank = rstPinBank;
    ads->rstPin     = rstPin;

    ads->status.spiHealthy = true;

    ADS8688_Reset(ads);

    if(ADS8688_SelfTest(ads) != HAL_OK)
    {
        return HAL_ERROR;
    }

    uint8_t data[2];

    data[0] = 0xFF;
    ADS_Prog_Write(ads,AUTO_SEQ_EN,data);

    data[0] = 0x03;
    ADS_Prog_Write(ads,FEATURE_SELECT,data);

    for(uint8_t i=0;i<8;i++)
    {
        data[0] = config->range[i];

        ADS_Prog_Write(
            ads,
            CHN_0_RANGE + i,
            data);
    }

    ADS_Cmd_Write(
        ads,
        AUTO_RST,
        data);

    ads->status.initialized = true;

    return HAL_OK;
}
/*
 * Read Program Register
 */
HAL_StatusTypeDef ADS_Prog_Read(
    ADS8688 *ads,
    uint8_t addr,
    uint8_t *data)
{
    HAL_StatusTypeDef ret;

    uint8_t txbuf[2] =
    {
        0x00,
        (uint8_t)((addr << 1U) & 0xFEU)
    };

    uint8_t rxbuf[2] = {0};

    if((ads == NULL) || (data == NULL))
    {
        return HAL_ERROR;
    }

    HAL_GPIO_WritePin(
        ads->csPinBank,
        ads->csPin,
        GPIO_PIN_RESET);

    ret = HAL_SPI_TransmitReceive(
        ads->spiHandle,
        txbuf,
        rxbuf,
        2,
        10);

    HAL_GPIO_WritePin(
        ads->csPinBank,
        ads->csPin,
        GPIO_PIN_SET);

    if(ret != HAL_OK)
    {
        return ret;
    }

    data[0] = rxbuf[0];
    data[1] = rxbuf[1];

    return HAL_OK;
}

/*
 * Write Program Register
 */
HAL_StatusTypeDef ADS_Prog_Write(
    ADS8688 *ads,
    uint8_t addr,
    uint8_t *data)
{
    HAL_StatusTypeDef ret;

    uint8_t txbuf[2] =
    {
        data[0],
        (uint8_t)((addr << 1U) | 0x01U)
    };

    uint8_t rxbuf[2] = {0};

    if((ads == NULL) || (data == NULL))
    {
        return HAL_ERROR;
    }

    HAL_GPIO_WritePin(
        ads->csPinBank,
        ads->csPin,
        GPIO_PIN_RESET);

    ret = HAL_SPI_TransmitReceive(
        ads->spiHandle,
        txbuf,
        rxbuf,
        2,
        10);

    HAL_GPIO_WritePin(
        ads->csPinBank,
        ads->csPin,
        GPIO_PIN_SET);

    return ret;
}

/*
 * Send Command
 */
HAL_StatusTypeDef ADS_Cmd_Write(
    ADS8688 *ads,
    uint8_t cmd,
    uint8_t *data)
{
    HAL_StatusTypeDef ret;

    uint8_t txbuf[2] =
    {
        0x00,
        cmd
    };

    uint8_t rxbuf[2] = {0};

    if((ads == NULL) || (data == NULL))
    {
        return HAL_ERROR;
    }

    HAL_GPIO_WritePin(
        ads->csPinBank,
        ads->csPin,
        GPIO_PIN_RESET);

    ret = HAL_SPI_TransmitReceive(
        ads->spiHandle,
        txbuf,
        rxbuf,
        2,
        10);

    HAL_GPIO_WritePin(
        ads->csPinBank,
        ads->csPin,
        GPIO_PIN_SET);

    if(ret != HAL_OK)
    {
        return ret;
    }

    data[0] = rxbuf[0];
    data[1] = rxbuf[1];

    return HAL_OK;
}

/*
 * Read All Enabled Channels
 */
HAL_StatusTypeDef ADS_Read_All_Raw(
    ADS8688 *ads,
    uint16_t *data)
{
    HAL_StatusTypeDef ret;

    uint8_t ads_raw[2] = {0};

    if((ads == NULL) || (data == NULL))
    {
        return HAL_ERROR;
    }

    for(uint8_t i = 0; i <ADS8688_CHANNELS; i++)
    {
        ret = ADS_Cmd_Write(
            ads,
            CONT,
            ads_raw);

        if(ret != HAL_OK)
        {
            return ret;
        }

        data[i] =
            ((((uint16_t)ads_raw[1]) << 8U)
            | ((uint16_t)ads_raw[0])) >> 4U;
    }

    return HAL_OK;
}
