#ifndef __ADS8688_INTERFACE_H
#define __ADS8688_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "analog_input.h"
#include "ADS8688.h"

/*=========================================================
  EXPORTED DRIVER
=========================================================*/

extern ANALOG_INPUT_DrvTypeDef ADS8688Drv;

/*=========================================================
  REGISTRATION
=========================================================*/

void ADS8688_RegisterDevice(
    ADS8688 *ads);

#ifdef __cplusplus
}
#endif

#endif
