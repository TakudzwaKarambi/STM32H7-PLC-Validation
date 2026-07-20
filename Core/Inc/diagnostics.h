#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/*=========================================================
  DIAGNOSTIC FAULTS
=========================================================*/

typedef enum
{
    DIAG_FAULT_NONE = 0,

    DIAG_FAULT_CLT01,
    DIAG_FAULT_VNI8200,
    DIAG_FAULT_ADS8688,

    DIAG_FAULT_ARC_COMM,
    DIAG_FAULT_UV_COMM,
    DIAG_FAULT_ETHERNET,

    DIAG_FAULT_PROCESS_IMAGE,

    DIAG_FAULT_UNKNOWN

} DiagnosticFault_t;

/*=========================================================
  DIAGNOSTIC STATUS
=========================================================*/

typedef struct
{
    bool systemHealthy;

    bool clt01Fault;
    bool vni8200Fault;
    bool ads8688Fault;

    bool arcCommFault;
    bool uvCommFault;
    bool ethernetFault;

    DiagnosticFault_t lastFault;

} DiagnosticStatus_t;

/*=========================================================
  GLOBAL DIAGNOSTIC OBJECT
=========================================================*/

extern DiagnosticStatus_t Diagnostics;

/*=========================================================
  API
=========================================================*/

void Diagnostics_Init(void);

void Diagnostics_SetFault(DiagnosticFault_t fault);

void Diagnostics_ClearFault(DiagnosticFault_t fault);

bool Diagnostics_IsHealthy(void);

#ifdef __cplusplus
}
#endif

#endif