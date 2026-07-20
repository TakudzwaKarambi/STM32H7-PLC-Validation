#ifndef DAC81408_INTERFACE_H
#define DAC81408_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dac81408.h"
#include "analog_output.h"

extern ANALOG_OUTPUT_DrvTypeDef DAC81408Drv;

void DAC81408_RegisterDevice(
    DAC81408_t *dac);

#ifdef __cplusplus
}
#endif

#endif
