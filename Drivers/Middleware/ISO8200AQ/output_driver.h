#ifndef OUTPUT_DRIVER_H
#define OUTPUT_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t OutputBitmap;

    bool PowerGood;

    bool Fault;

    bool OutputEnabled;

    uint8_t FaultRegister;

} OutputDriver_Status_t;


/*------------------------------------------------------------------*/
/* Initialization                                                   */
/*------------------------------------------------------------------*/

void OutputDriver_Init(void);


/*------------------------------------------------------------------*/
/* Output Control                                                   */
/*------------------------------------------------------------------*/

bool OutputDriver_Write(uint8_t bitmap);

bool OutputDriver_SetChannel(
    uint8_t channel,
    bool state);

uint8_t OutputDriver_GetOutputs(void);


/*------------------------------------------------------------------*/
/* Diagnostics                                                     */
/*------------------------------------------------------------------*/

bool OutputDriver_ReadStatus(
    OutputDriver_Status_t *status);

bool OutputDriver_ClearFaults(void);


/*------------------------------------------------------------------*/
/* Enable / Disable                                                 */
/*------------------------------------------------------------------*/

bool OutputDriver_Enable(void);

bool OutputDriver_Disable(void);

bool OutputDriver_IsEnabled(void);

#ifdef __cplusplus
}
#endif

#endif
