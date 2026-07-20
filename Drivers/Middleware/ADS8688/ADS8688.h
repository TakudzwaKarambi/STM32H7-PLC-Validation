#ifndef ADS8688_H
#define ADS8688_H

#include "stm32h7xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

/* Commands */
#define CONT        0x00
#define STBY        0x82
#define PWDN        0x83
#define RST         0x85
#define AUTO_RST    0xA0

#define MAN_0       0xC0
#define MAN_1       0xC4
#define MAN_2       0xC8
#define MAN_3       0xCC
#define MAN_4       0xD0
#define MAN_5       0xD4
#define MAN_6       0xD8
#define MAN_7       0xDC

/* Registers */
#define AUTO_SEQ_EN       0x01
#define CHN_PWRDN         0x02
#define FEATURE_SELECT    0x03

#define CHN_0_RANGE       0x05
#define CHN_1_RANGE       0x06
#define CHN_2_RANGE       0x07
#define CHN_3_RANGE       0x08
#define CHN_4_RANGE       0x09
#define CHN_5_RANGE       0x0A
#define CHN_6_RANGE       0x0B
#define CHN_7_RANGE       0x0C

#define CMD_RD_BCK        0x3F

#define ADS8688_CHANNELS  8

typedef struct
{
    uint8_t range[ADS8688_CHANNELS];
} ADS8688_Config_t;

typedef struct
{
    bool spiHealthy;
    bool initialized;
    uint32_t commErrors;
} ADS8688_Status_t;

typedef struct
{
    SPI_HandleTypeDef *spiHandle;

    GPIO_TypeDef *csPinBank;
    uint16_t csPin;

    GPIO_TypeDef *rstPinBank;
    uint16_t rstPin;

    ADS8688_Status_t status;

} ADS8688;

HAL_StatusTypeDef ADS8688_Init(
    ADS8688 *ads,
    SPI_HandleTypeDef *spiHandle,
    GPIO_TypeDef *csPinBank,
    uint16_t csPin,
    GPIO_TypeDef *rstPinBank,
    uint16_t rstPin,
    const ADS8688_Config_t *config);

HAL_StatusTypeDef ADS8688_Reset(
    ADS8688 *ads);

HAL_StatusTypeDef ADS8688_SelfTest(
    ADS8688 *ads);

HAL_StatusTypeDef ADS_Prog_Read(
    ADS8688 *ads,
    uint8_t addr,
    uint8_t *data);

HAL_StatusTypeDef ADS_Prog_Write(
    ADS8688 *ads,
    uint8_t addr,
    uint8_t *data);

HAL_StatusTypeDef ADS_Cmd_Write(
    ADS8688 *ads,
    uint8_t cmd,
    uint8_t *data);

HAL_StatusTypeDef ADS_Read_All_Raw(
    ADS8688 *ads,
    uint16_t *data);

const ADS8688_Status_t* ADS8688_GetStatus(
    ADS8688 *ads);

HAL_StatusTypeDef ADS_SPI_Transfer(ADS8688 *ads, uint8_t *tx, uint8_t *rx, uint16_t len);

#endif
