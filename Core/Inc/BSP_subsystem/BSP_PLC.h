#ifndef BSP_PLC_H
#define BSP_PLC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "process_image.h"

/*=========================================================
  INITIALIZATION
=========================================================*/

bool BSP_PLC_Init(void);

/*=========================================================
  INPUT SCAN TASKS
=========================================================*/
void BSP_LED_On(void);
void BSP_LED_Off(void);
void BSP_LED_Toggle(void);
/* Digital Inputs (CLT01-38SQ7) */
void BSP_PLC_UpdateInputs(void);

/* Analog Inputs (ADS8688) */
void BSP_PLC_UpdateAnalogInputs(void);

/*=========================================================
  OUTPUT SCAN TASKS
=========================================================*/

/* Digital Outputs (ISO8200AQ) */
void BSP_PLC_UpdateOutputs(void);

/* Analog Outputs (DAC81408) */
void BSP_PLC_UpdateAnalogOutputs(void);

/*=========================================================
  DIAGNOSTICS
=========================================================*/

void BSP_PLC_UpdateDiagnostics(void);

/*=========================================================
  DRIVER STATUS
=========================================================*/

bool BSP_PLC_IsInitialized(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_PLC_H */
