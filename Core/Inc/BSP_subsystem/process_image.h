#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*=========================================================
  SYSTEM STATUS
=========================================================*/

typedef struct
{
    bool plcRunning;
    bool systemHealthy;

    /* Communication */

    bool arcCommFault;
    bool uvCommFault;
    bool ethernetFault;

    /* Hardware Drivers */

    bool ads8688Fault;
    bool dac81408Fault;

    bool clt01Fault;
    bool iso8200Fault;

} SystemStatus_t;

/*=========================================================
  DIGITAL INPUTS
=========================================================*/

typedef struct
{
    bool LEH;
    bool LEL;

    bool AS01;
    bool AS02;

    bool PT01_Valid;
    bool PT02_Valid;
    bool PT03_Valid;

    bool FM01_Valid;
    bool FM02_Valid;

} DigitalInputs_t;

/*=========================================================
  ANALOG INPUTS
=========================================================*/

typedef struct
{
    float PT01;
    float PT02;
    float PT03;

    float FM01;
    float FM02;

} AnalogInputs_t;

/*=========================================================
  HAMILTON ARC SENSORS
=========================================================*/

typedef struct
{
    float pH01;
    float pH02;

    float CM01;
    float CM02;

    bool pH01_Valid;
    bool pH02_Valid;

    bool CM01_Valid;
    bool CM02_Valid;

} ArcInputs_t;

/*=========================================================
  UV DETECTOR
=========================================================*/

typedef struct
{
    float absorbance;

    bool valid;
    bool communicationOk;

} UVInputs_t;

/*=========================================================
  DIGITAL OUTPUTS
=========================================================*/

typedef struct
{
    /* Solenoid Valves */

    bool SA1;
    bool SA2;
    bool SA3;
    bool SA4;

    bool SB1;
    bool SB2;
    bool SB3;
    bool SB4;

    /* Process Valves */

    bool V01;
    bool V02;
    bool V03;
    bool V04;
    bool V05;
    bool V06;
    bool V07;

    /* UV Trigger */

    bool VT01;

    /* Fans */

    bool FA1;
    bool FA2;
    bool FA3;
    bool FA4;

    /* Pumps */

    bool P01A;
    bool P01B;

} DigitalOutputs_t;

/*=========================================================
  ANALOG OUTPUTS
=========================================================*/

typedef struct
{
    float AO01;
    float AO02;
    float AO03;
    float AO04;

    float AO05;
    float AO06;
    float AO07;
    float AO08;

} AnalogOutputs_t;

/*=========================================================
  INPUT IMAGE
=========================================================*/

typedef struct
{
    DigitalInputs_t digital;
    AnalogInputs_t analog;

    ArcInputs_t arc;
    UVInputs_t uv;

} InputImage_t;

/*=========================================================
  OUTPUT IMAGE
=========================================================*/

typedef struct
{
    DigitalOutputs_t digital;
    AnalogOutputs_t analog;

} OutputImage_t;

/*=========================================================
  SCAN STATISTICS
=========================================================*/

typedef struct
{
    uint32_t scanCounter;

    uint32_t lastScanTime_us;
    uint32_t minScanTime_us;
    uint32_t maxScanTime_us;

} ScanStatistics_t;

/*=========================================================
  GLOBAL PROCESS IMAGE
=========================================================*/

extern InputImage_t InputImage;

extern OutputImage_t OutputImage;

extern SystemStatus_t SystemStatus;

extern ScanStatistics_t ScanStats;

/*=========================================================
  FUNCTIONS
=========================================================*/

void ProcessImage_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* PROCESS_IMAGE_H */
