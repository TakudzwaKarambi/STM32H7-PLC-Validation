
#include "analog_input.h"
#include "main.h"
#include "ADS8688.h"
#include "ADS8688_Interface.h"
static ANALOG_INPUT_DrvTypeDef *AnalogDrv;



void BSP_ANALOG_RegisterDriver(
    ANALOG_INPUT_DrvTypeDef *drv)
{
    AnalogDrv = drv;
}



ANALOG_INPUT_StatusTypeDef BSP_ANALOG_Init(
    ANALOG_INPUT_InitTypeDef *init)
{
    if(AnalogDrv == 0)
    {
        return ANALOG_INPUT_ERROR;
    }

    return AnalogDrv->Init(init);
}



ANALOG_INPUT_StatusTypeDef BSP_ANALOG_Read(
    uint16_t *data)
{
    if(AnalogDrv == 0)
    {
        return ANALOG_INPUT_ERROR;
    }

    return AnalogDrv->ReadRaw(data);
}


ANALOG_INPUT_StatusTypeDef BSP_ANALOG_GetStatus(
    uint8_t *status)
{
    if(AnalogDrv == 0)
    {
        return ANALOG_INPUT_ERROR;
    }

    return AnalogDrv->GetStatus(status);
}






bool ANALOG_INPUT_TestSPI(SPI_HandleTypeDef *hspi)
{
    ADS8688 testDevice = {0};
    uint8_t readData[2] = {0};
    uint8_t writeData[2] = {0};

    // 1. Populate pin setup
    testDevice.spiHandle  = hspi;
    testDevice.csPinBank  = ADS_CS_GPIO_Port;
    testDevice.csPin      = ADS_CS_Pin;
    testDevice.rstPinBank = ADS_RST_GPIO_Port;
    testDevice.rstPin     = ADS_RST_Pin;

    // 2. Hardware Reset
    if (ADS8688_Reset(&testDevice) != HAL_OK)
    {
        return false;
    }

    // 3. Write test pattern (0x03) to FEATURE_SELECT
    writeData[0] = 0x03;
    if (ADS_Prog_Write(&testDevice, FEATURE_SELECT, writeData) != HAL_OK)
    {
        return false;
    }

    // 4. Read back register
    if (ADS_Prog_Read(&testDevice, FEATURE_SELECT, readData) != HAL_OK)
    {
        return false;
    }

    // 5. Check if returned data matches 0x03
    return (readData[0] == 0x03);
}
