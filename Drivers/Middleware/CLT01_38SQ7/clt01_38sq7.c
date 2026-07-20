#include "clt01_38sq7.h"

#define CLT_STOP_HIGH_MASK     0x0001
#define CLT_STOP_LOW_MASK      0x0002
#define CLT_OTA_MASK           0x0040
#define CLT_UVA_MASK           0x0080

static inline void CLT_CS_LOW(CLT01_Handle_t *hclt)
{
    HAL_GPIO_WritePin(hclt->CS_Port, hclt->CS_Pin, GPIO_PIN_RESET);
}

static inline void CLT_CS_HIGH(CLT01_Handle_t *hclt)
{
    HAL_GPIO_WritePin(hclt->CS_Port, hclt->CS_Pin, GPIO_PIN_SET);
}

static bool CLT_CheckParity(uint16_t frame)
{
    uint8_t in1 = (frame >> 8) & 0xFF;
    uint8_t pc1 = (frame >> 5) & 0x01;
    uint8_t pc2 = (frame >> 4) & 0x01;
    uint8_t pc3 = (frame >> 3) & 0x01;
    uint8_t pc4 = (frame >> 2) & 0x01;

    bool calc1 = !(((in1>>7)&1)^((in1>>6)&1));
    bool calc2 = !(((in1>>5)&1)^((in1>>4)&1));
    bool calc3 = !(((in1>>3)&1)^((in1>>2)&1));
    bool calc4 = !(((in1>>1)&1)^((in1>>0)&1));

    return (pc1 == calc1) && (pc2 == calc2) && (pc3 == calc3) && (pc4 == calc4);
}

HAL_StatusTypeDef CLT01_Init(CLT01_Handle_t *hclt)
{
    /* Deselect SPI device initially */
    CLT_CS_HIGH(hclt);

    /* Turn ON the chip by driving DI_EN High */
    if (hclt->EN_Port != NULL)
    {
        HAL_GPIO_WritePin(hclt->EN_Port, hclt->EN_Pin, GPIO_PIN_SET);
    }

    /* Wait for power-on stabilization time */
    HAL_Delay(10);

    return HAL_OK;
}

HAL_StatusTypeDef CLT01_ReadStatus(CLT01_Handle_t *hclt, CLT01_Status_t *status)
{
    uint8_t tx[2] = {0x00, 0x00};
    uint8_t rx[2] = {0x00, 0x00};

    CLT_CS_LOW(hclt);
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(hclt->hspi, tx, rx, 2, 100);
    CLT_CS_HIGH(hclt);

    if(ret != HAL_OK) return ret;

    uint16_t frame = ((uint16_t)rx[0] << 8) | (uint16_t)rx[1];

    status->RawFrame        = frame;
    status->Inputs          = (frame >> 8) & 0xFF;
    status->UnderVoltage    = ((frame & CLT_UVA_MASK) == 0);
    status->OverTemperature = ((frame & CLT_OTA_MASK) == 0);
    status->StopBitError    = ((frame & CLT_STOP_HIGH_MASK) == 0);
    status->ParityError     = !CLT_CheckParity(frame);

    return HAL_OK;
}

bool CLT01_IsAlive(const CLT01_Status_t *status)
{
    /* If the frame reads completely 0x0000 or 0xFFFF, SPI communication is broken */
    if (status->RawFrame == 0x0000 || status->RawFrame == 0xFFFF)
        return false;

    if (status->StopBitError || status->ParityError)
        return false;

    return true;
}

HAL_StatusTypeDef CLT01_TestCommunication(CLT01_Handle_t *hclt, uint16_t *rawFrame)
{
    uint8_t tx[2] = {0x00, 0x00};
    uint8_t rx[2] = {0x00, 0x00};

    CLT_CS_LOW(hclt);
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(hclt->hspi, tx, rx, 2, 100);
    CLT_CS_HIGH(hclt);

    if(ret != HAL_OK) return ret;

    *rawFrame = ((uint16_t)rx[0] << 8) | (uint16_t)rx[1];
    return HAL_OK;
}
