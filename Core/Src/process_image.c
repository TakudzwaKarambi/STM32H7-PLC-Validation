#include "process_image.h"

#include <string.h>

/*=========================================================
  GLOBAL PROCESS IMAGE INSTANCES
=========================================================*/

InputImage_t InputImage;

OutputImage_t OutputImage;

SystemStatus_t SystemStatus;

ScanStatistics_t ScanStats;

/*=========================================================
  INITIALIZATION
=========================================================*/

void ProcessImage_Init(void)
{
    memset(&InputImage,
           0,
           sizeof(InputImage));

    memset(&OutputImage,
           0,
           sizeof(OutputImage));

    memset(&SystemStatus,
           0,
           sizeof(SystemStatus));

    memset(&ScanStats,
           0,
           sizeof(ScanStats));

    /*---------------------------------------------
      System Status Defaults
    ---------------------------------------------*/

    SystemStatus.plcRunning = false;

    SystemStatus.systemHealthy = true;

    SystemStatus.arcCommFault = false;
    SystemStatus.uvCommFault = false;
    SystemStatus.ethernetFault = false;

    SystemStatus.ads8688Fault = false;
    SystemStatus.dac81408Fault = false;

    SystemStatus.clt01Fault = false;
    SystemStatus.iso8200Fault = false;

    /*---------------------------------------------
      Scan Statistics Defaults
    ---------------------------------------------*/

    ScanStats.scanCounter = 0U;

    ScanStats.lastScanTime_us = 0U;

    ScanStats.minScanTime_us = 0xFFFFFFFFU;

    ScanStats.maxScanTime_us = 0U;
}
