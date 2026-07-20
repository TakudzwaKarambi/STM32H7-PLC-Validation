/**
  ******************************************************************************
  * @file    iso8200aq.c
  * @author  IPC Rennes
  * @version V1.0.0
  * @date    March 21th, 2017
  * @brief   ISO8200AQ driver (galvanic isolated 8 channels switch driver)
  * @note    (C) COPYRIGHT 2017 STMicroelectronics
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#include "iso8200aq.h"
#include "iso8200aq.h"

#ifndef TRUE
#define TRUE  true
#endif

#ifndef FALSE
#define FALSE false
#endif
/* Private constants  ---------------------------------------------------------*/
    
/** @addtogroup BSP
  * @{
  */   
   
/** @defgroup ISO8200AQ ISO8200AQ
  * @{
  */   

/* Private constants ---------------------------------------------------------*/    

/** @defgroup ISO8200AQ_Private_Constants ISO8200AQ Private Constants
  * @{
  */   

/** Error while initialising the SPI */
#define ISO8200AQ_ERROR_0   (0x8000)   
/** Error: while initialising timer */
#define ISO8200AQ_ERROR_1   (0x8001)
/** Error: while setting channel DC */
#define ISO8200AQ_ERROR_2   (0x8002)
/** Error: while setting channel DC */
#define ISO8200AQ_ERROR_3   (0x8003)
/** Error: while queueing bitmap*/
#define ISO8200AQ_ERROR_4   (0x8004)
/** Error: while queueing bitmap*/
#define ISO8200AQ_ERROR_5   (0x8005)
/** Error: bad SPI transaction */
#define ISO8200AQ_ERROR_6   (0x8006)

/** PWM Timer frequency in Hz */
#define ISO8200AQ_PWM_TIMER_FREQ (10000U) /* Minimum value is 10000 to support 1/100 duty cycle at 100Hz */
                                             
/** Guard Timer frequency in Hz */
#define ISO8200AQ_GUARD_TIMER_FREQ (100000U) /* Minimum value is 100000 to generate a time base of 10 us*/

/** Guard Timer tick in us */
#define ISO8200AQ_GUARD_TIMER_TICK_IN_US (1000000U/ISO8200AQ_GUARD_TIMER_FREQ)    

/** Maximum channel frequency in tenth of Hz */
#define ISO8200AQ_MAX_CHAN_FREQ (1000U)
    
/**
  * @}
  */ 
    
/* Private variables ---------------------------------------------------------*/

/** @defgroup ISO8200AQ_Private_Variables ISO8200AQ Private Variables
  * @{
  */       

/** Function pointer to error handler call back */
static void (*pErrorHandlerCallback)(uint16_t);
/** Function pointer to fault interrupt call back */
static void (*pFaultInterruptCallback)(uint8_t);
/** Function pointer to power good interrupt call back */
static void (*pPGoodInterruptCallback)(uint8_t);

/** Channel action table */
static uint32_t chanPwmTimActionTable[MAX_NUMBER_OF_DEVICES][ISO8200AQ_NB_CHANNELS];
/** Rising edges channel table */
static volatile uint32_t chanPwmTimPeriodHigh[MAX_NUMBER_OF_DEVICES][ISO8200AQ_NB_CHANNELS];
/** Falling edges channel table */
static uint32_t chanPwmTimPeriodLow[MAX_NUMBER_OF_DEVICES][ISO8200AQ_NB_CHANNELS];
/** forceResync flag: set to force channel update */
static volatile bool forceResync = FALSE;
/** Number of ISO8200aq driver instance */
static uint16_t iso8200aqDriverInstance = 0;
/** iso8200aqPwmEnable flag: set when the periodical ISO8200aq channels updates is enabled*/
static volatile uint8_t iso8200aqPwmEnable = FALSE;
/** ISR flag : set when code an ISR is running */
static volatile bool isrFlag = FALSE;
/** Number of ISO8200aq devices */
static uint8_t numberOfDevices;
/** pwmTimNextAction is the tick value when the next channel update will occur */
static  uint32_t pwmTimNextAction;
/** Tick counter  */
static volatile uint32_t pwmTimTickCnt;
/** queuedBitmap flag which is set when a new channel bitmap has to be applied  */
static bool queuedBitmap = FALSE;
/** SPI preemption flag : set when an SPI access occures under ISR*/
static volatile bool spiPreemtionByIsr = FALSE;
/** SPI Lock time in us between two consecutive SPI access */
static volatile int16_t spiLockTime;
/** SPI Rx buffer */
static uint8_t spiRxBursts[MAX_NUMBER_OF_DEVICES];
/** SPI Tx buffer */
static uint8_t spiTxBursts[MAX_NUMBER_OF_DEVICES];
/** ISO8200AQ Driver Paramaters structure */
static driverParams_t driverPrm;

/**
  * @}
  */ 

/* Private function prototypes -----------------------------------------------*/

/** @defgroup ISO8200AQ_Private_functions ISO8200AQ Private functions
  * @{
  */  
void Iso8200aq_ErrorHandler(uint16_t error);
void Iso8200aq_FaultInterruptHandler(uint8_t);                      
void Iso8200aq_GuardTimerTick(void);
void Iso8200aq_PGoodInterruptHandler(uint8_t);
void Iso8200aq_PwmTimerTick(void);
void Iso8200aq_WriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte);    
void Iso8200aq_SetDeviceParamsToPredefinedValues(uint8_t deviceId);
void Iso8200aq_SetDeviceParamsToGivenValues(uint8_t deviceId, iso8200aq_Init_t *pInitPrm);

/**
  * @}
  */ 


/** @defgroup ISO8200AQ_Exported_Variables ISO8200AQ Exported Variables
  * @{
  */       

/** ISO8200AQ relay driver functions pointer structure  */

/**
  * @}
  */ 

/** @defgroup ISO8200AQ_Library_Functions ISO8200AQ Library Functions
  * @{
  */   

/******************************************************//**
 * @brief  Attaches a user callback to the error Handler.
 * The call back will be then called each time the library 
 * detects an error
 * @param[in] callback Name of the callback to attach 
 * to the error Hanlder
 * @retval None
 **********************************************************/
void Iso8200aq_AttachErrorHandler(void (*callback)(uint16_t))
{
  pErrorHandlerCallback = (void (*)(uint16_t))callback;
}

/******************************************************//**
 * @brief  Attaches a user callback to the Fault interrupt
 * The call back will be then called each time the fault 
 * pin will be pulled down/pull up due to the 
 * occurrence/clearance of a common fault (OVL, OVT)
 * @param[in] callback Name of the callback to attach 
 * to the Fault interrupt
 * @retval None
 **********************************************************/
void Iso8200aq_AttachFaultInterrupt(void (*callback)(uint8_t))
{
  pFaultInterruptCallback = (void (*)(uint8_t))callback;
}

/******************************************************//**
 * @brief  Attaches a user callback to the PGood interrupt
 * The call back will be then called each time the power 
 * good diagnostic pin will be pulled down/pull up 
 * @param[in] callback name of the callback to attach 
 * to the PGood interrupt
 * @retval None
 **********************************************************/
void Iso8200aq_AttachPGoodInterrupt(void (*callback)(uint8_t))
{
  pPGoodInterruptCallback = (void (*)(uint8_t))callback;
}

/******************************************************//**
 * @brief  Get status of all channels of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval channel status bitmap
 **********************************************************/
uint8_t Iso8200aq_GetAllChannelStatus(uint8_t deviceId)
{
  uint8_t chanStatus = 0;
  if (deviceId <= (iso8200aqDriverInstance - 1U ))
  {  
    chanStatus = driverPrm.device[deviceId].chanEnBitmap;    
  }
  return (chanStatus);
}

/******************************************************//**
 * @brief  Get Channel duty cycle
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @param[in] chanId (from 0 to ISO8200AQ_NB_CHANNELS-1)
 * @retval channel duty cycle from 0 to 100
 **********************************************************/
uint8_t Iso8200aq_GetChannelDc(uint8_t deviceId, uint8_t chanId)
{
  return (driverPrm.device[deviceId].channelDc[chanId]);   
}
             
/******************************************************//**
 * @brief  Get Channel frequency
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @param[in] chanId (from 0 to ISO8200AQ_NB_CHANNELS-1)
 * @retval channel frequency in 1/10Hz
 **********************************************************/
uint16_t Iso8200aq_GetChannelFreq(uint8_t deviceId, uint8_t chanId)
{
  return (driverPrm.device[deviceId].channelFreq[chanId]);   
}
                                                                   
/******************************************************//**
 * @brief  Get Fault status register of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval Fault register status
 **********************************************************/
uint8_t Iso8200aq_GetFaultRegister(uint8_t deviceId)
{
  uint8_t faultRegister = 0; 
  if (deviceId <= (iso8200aqDriverInstance - 1U ))
  {
    uint32_t i;
    bool itDisable = FALSE;  
    if (iso8200aqPwmEnable == FALSE)
    {
      do
      {
        spiPreemtionByIsr = FALSE;
        if (itDisable)
        {
          /* re-enable Iso8200aq_Board_EnableIrq if disable in previous iteration */
          Iso8200aq_Board_EnableIrq();
          itDisable = FALSE;
        }
        
        for (i = 0; i < numberOfDevices; i++)
        {
          spiTxBursts[numberOfDevices - 1U - i] = driverPrm.device[i].chanEnBitmap;
        }
        
        /* Wait for the SPI lock time is elapsed before sending a new request */
        while (spiLockTime > 0)
        {
        }
        
        /* Disable interruption before checking */
        /* pre-emption by ISR and SPI transfers*/
        Iso8200aq_Board_DisableIrq();
        itDisable = TRUE;
      } while (spiPreemtionByIsr); /* check pre-emption by ISR */
    
      Iso8200aq_WriteBytes(&spiTxBursts[0], &spiRxBursts[0]); 
    }
    faultRegister = spiRxBursts[numberOfDevices - 1U - deviceId];
    /* re-enable Iso8200aq_Board_EnableIrq after SPI transfers*/
    Iso8200aq_Board_EnableIrq();
  }
  return (faultRegister);
} 

/******************************************************//**
 * @brief  Get Fault pin status  of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval Fault pin status
 **********************************************************/
uint8_t Iso8200aq_GetFaultStatus(uint8_t deviceId)
{
  uint8_t state = 0; 
  if (deviceId <= (iso8200aqDriverInstance - 1U ))
  {
    if (Iso8200aq_Board_GetFaultPinState(deviceId) != 0U)
    {
      state = 0U;  
    }
    else
    {
      state = 1U;        
    }
  }
    return (state);
} 

/******************************************************//**
 * @brief Returns the FW version of the library
 * @retval ISO8200AQ_FW_VERSION
 **********************************************************/
uint32_t Iso8200aq_GetFwVersion(void)
{
  return (ISO8200AQ_FW_VERSION);
}


/******************************************************//**
 * @brief  Returns the number of devices
 * @retval number of devices
 **********************************************************/
uint8_t Iso8200aq_GetNbDevices(void)
{
  return (numberOfDevices);
}

/******************************************************//**
 * @brief  Get status of one channels of a specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @param[in] chanId (from 0 to ISO8200AQ_NB_CHANNELS-1)
 * @retval channel status 
 **********************************************************/
uint8_t Iso8200aq_GetOneChannelStatus(uint8_t deviceId, uint8_t chanId)
{
  uint8_t chanStatus = 0;
  if ((deviceId <= (iso8200aqDriverInstance - 1U )) && 
      (chanId < ISO8200AQ_NB_CHANNELS))
  {  
    chanStatus = (driverPrm.device[deviceId].chanEnBitmap >> chanId) & 0x1U ;    
  }
  return (chanStatus);
}

/******************************************************//**
 * @brief  Get the OUT_EN pin state of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval state (1 if enable, 0 if disable)
 **********************************************************/
uint8_t Iso8200aq_GetOutEnable(uint8_t deviceId)
{
  uint8_t state = 0; 
  if (deviceId <= (iso8200aqDriverInstance - 1U ))
  {
    state = Iso8200aq_Board_GetOutPinState(deviceId);
  }
  return (state);
}

/******************************************************//**
 * @brief  Get PGood  pin status  of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval PGood pin status
 **********************************************************/
uint8_t Iso8200aq_GetPGoodStatus(uint8_t deviceId)
{
  uint8_t state = 0; 
  if (deviceId <= (iso8200aqDriverInstance - 1U ))
  {
    if (Iso8200aq_Board_GetPGoodPinState(deviceId) != 0U)
    {
      state = 0U;      
    }
    else
    {
      state = 1U;
    }
  }
  return (state);
}

/******************************************************//**
 * @brief  Get PWM enable state 
 * @retval Pwm enable state (1 if enable, 0 if disable)
 **********************************************************/
uint8_t Iso8200aq_GetPwmEnable(void)
{
  return (iso8200aqPwmEnable);
}

/******************************************************//**
 * @brief Return relay handle (pointer to the ISO8200AQ relay driver structure)
 * @retval Pointer to the relayDrv_t structure
 **********************************************************/


/******************************************************//**
 * @brief  Get the SPI frequency 
 * @retval SPI frequency in kHz
 **********************************************************/ 
uint32_t Iso8200aq_GetSpiFreq(void)
{
  return (driverPrm.spiFreq);
}

/******************************************************//**
 * @brief  Get timing in us
 * @param[in] timingId Id of the targeted timing
 * @retval timing in us
 **********************************************************/
uint8_t Iso8200aq_GetTiming(uint8_t timingId)
{
  uint8_t timing = 0;
  if (timingId == 0U)
  {
    timing = driverPrm.timingTcss;
  }
  return (timing);
}

/******************************************************//**
 * @brief Starts a new ISO8200AQ instance 
 * @param[in] pInit pointer to the initialization data
 * @retval None
 **********************************************************/
void Iso8200aq_Init(void* pInit)
{
  /* Set all registers and context variables to the predefined values from iso8200aq_target_config.h */
  if (pInit == (void*)0)
  {
    Iso8200aq_SetDeviceParamsToPredefinedValues((uint8_t)iso8200aqDriverInstance);
  }
  else
  {
    Iso8200aq_SetDeviceParamsToGivenValues((uint8_t)iso8200aqDriverInstance, pInit);
  }

  /* Initialise the GPIOs */
  Iso8200aq_Board_GpioInit((uint8_t)iso8200aqDriverInstance);
  
  if (Iso8200aq_Board_SpiInit(driverPrm.spiFreq) != 0U)
  {
    /* Spi initialization Error */
    Iso8200aq_ErrorHandler(ISO8200AQ_ERROR_0);
  } 

  /* Initialise the PWM Timer */
  if (Iso8200aq_Board_TimerInit(ISO8200AQ_PWM_TIMER_FREQ, ISO8200AQ_GUARD_TIMER_FREQ) != 0U)
  {
     /* Timer initialization Error */
    Iso8200aq_ErrorHandler(ISO8200AQ_ERROR_1);
  }
 
  /* Let a delay after reset */
  Iso8200aq_Board_Delay(1);                     

  /* Disable outputs */
  Iso8200aq_Board_OutDisable((uint8_t)iso8200aqDriverInstance);

  iso8200aqDriverInstance++;
}

/******************************************************//**
 * @brief Put a channel bitamp in queue before synchronous sending
 * done by calling Iso8200aq_SendQueuedBitmaps.
 * Any call to functions that use the SPI between the calls of 
 * Iso8200aq_QueueChannelBitmap and Iso8200aq_SendQueuedBitmaps 
 * will corrupt the queue.
 * A command for each device of the daisy chain must be 
 * specified before calling Iso8200aq_SendQueuedBitmaps.
 * @param[in] deviceId deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] newChanBitmap Channel bitmapt to queue 
 * @retval TRUE if the channels bitmap was queued
 *********************************************************/
bool Iso8200aq_QueueChannelBitmap(uint8_t deviceId,  uint8_t newChanBitmap)
{
  bool status = FALSE; 
  if (deviceId <= (iso8200aqDriverInstance - 1U ))
  {
    spiTxBursts[iso8200aqDriverInstance - 1U - deviceId] = newChanBitmap;
    status = TRUE;
  }
  queuedBitmap = TRUE;
  return (status);  
}

/******************************************************//**
 * @brief Read id
 * @retval Id of the ISO8200AQ Driver Instance
 **********************************************************/
uint16_t Iso8200aq_ReadId(void)
{
  return (iso8200aqDriverInstance);
}

/******************************************************//**
 * @brief Sends previously queued channel bitmaps 
 * simultenously to all devices
 * @retval TRUE if successfull, FALSE if failure
 *********************************************************/
bool Iso8200aq_SendQueuedChannelBitmap(void)
{
  bool status = FALSE; 
  if (queuedBitmap)
  {
    /* Wait for the SPI lock time is elapsed before sending a new request */
    while (spiLockTime > 0)
    {
    }

    Iso8200aq_Board_DisableIrq();
  
    int8_t loop;
    Iso8200aq_WriteBytes(&spiTxBursts[0], &spiRxBursts[0]); 
    
    for (loop = (int8_t)(numberOfDevices) - 1U; loop >= 0; loop--)
    {
      driverPrm.device[loop].chanEnBitmap = spiTxBursts[numberOfDevices - 1U - loop];
    }    
    Iso8200aq_Board_EnableIrq();
    status = TRUE;
  }
  return (status);  
}

/******************************************************//**
 * @brief  Set status of all channels via Output Status register
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @param[in] newChanBitmap new channel status bitmap
 * @retval TRUE if channels status update were done
 **********************************************************/
bool Iso8200aq_SetAllChannelStatus(uint8_t deviceId, uint8_t newChanBitmap)
{
  bool status = FALSE; 
  if (deviceId <= (iso8200aqDriverInstance - 1U ))
  {
    uint32_t i;
    bool itDisable = FALSE;  

    /* If the PWM mode is enabled, only force the DC of the channels */
    /* of the related device */
    if (iso8200aqPwmEnable != 0U)
    {
      int8_t loop;
      for (loop = (int8_t)ISO8200AQ_NB_CHANNELS - 1; loop >= 0; loop--)
      {
        if ((newChanBitmap & (1U << (uint8_t)loop)) != 0U)
        {
          if (Iso8200aq_SetChannelDc(deviceId, (uint8_t)loop, 100U) == 0)
          {
            Iso8200aq_ErrorHandler(ISO8200AQ_ERROR_2); 
          }
        }
        else
        {
          if (Iso8200aq_SetChannelDc(deviceId, (uint8_t)loop, 0U) == 0)
          {
           Iso8200aq_ErrorHandler(ISO8200AQ_ERROR_3); 
          }
        }
      }
    }
    else
    {
      do
      {
        spiPreemtionByIsr = FALSE;
        if (itDisable)
        {
          /* re-enable Iso8200aq_Board_EnableIrq if disable in previous iteration */
          Iso8200aq_Board_EnableIrq();
          itDisable = FALSE;
        }
      
        for (i = 0; i < numberOfDevices; i++)
        {
          if (i != deviceId)
          {
            spiTxBursts[numberOfDevices - i - 1U] = driverPrm.device[i].chanEnBitmap;
          }
          else
          {
            spiTxBursts[numberOfDevices - i - 1U] = newChanBitmap;    
          }
        }
          
        /* Wait for the SPI lock time is elapsed before sending a new request */
        while (spiLockTime > 0)
        {
        }
        
        /* Disable interruption before checking */
        /* pre-emption by ISR and SPI transfers*/
        Iso8200aq_Board_DisableIrq();
        itDisable = TRUE;
      } while (spiPreemtionByIsr); /* check pre-emption by ISR */

      Iso8200aq_WriteBytes(&spiTxBursts[0], &spiRxBursts[0]); 
      driverPrm.device[deviceId].chanEnBitmap = newChanBitmap;    
        
      /* re-enable Iso8200aq_Board_EnableIrq after SPI transfers*/
      Iso8200aq_Board_EnableIrq();
    }
    status = TRUE;
  }
  return (status);
}

/******************************************************//**
 * @brief  Set Channel duty cycle
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @param[in] chanId (from 0 to ISO8200AQ_NB_CHANNELS-1)
 * @param[in] newDc new duty cycle from 0 to 100
 * @retval TRUE if successfull, FALSE if failure
 **********************************************************/
bool Iso8200aq_SetChannelDc(uint8_t deviceId, uint8_t chanId, uint8_t newDc)
{
  bool status = FALSE;
  if (deviceId <= (iso8200aqDriverInstance - 1U ))
  {
    if (chanId < ISO8200AQ_NB_CHANNELS)
    {
      if (newDc > 100U)
      {
        newDc = 100U; 
      }
      if (driverPrm.device[deviceId].channelDc[chanId] != newDc)
      {  
        driverPrm.device[deviceId].channelDc[chanId] = newDc;   
        if (driverPrm.device[deviceId].channelFreq[chanId] != 0U)
        {
          uint32_t period = (uint32_t)(ISO8200AQ_PWM_TIMER_FREQ) * 10U / driverPrm.device[deviceId].channelFreq[chanId];
          chanPwmTimPeriodHigh[deviceId][chanId] = period * newDc / 100U;
          chanPwmTimPeriodLow[deviceId][chanId] = period - chanPwmTimPeriodHigh[deviceId][chanId];
        }
        else
        {
          if (newDc == 100U)
          {
            chanPwmTimPeriodHigh[deviceId][chanId] =  0xFFFFFFFFU;
          }
          else
          {
            chanPwmTimPeriodHigh[deviceId][chanId] = 0;
          }  
          chanPwmTimPeriodLow[deviceId][chanId] = 0;
          /* Force recynchronisation after 1 second delay*/
          if (driverPrm.device[deviceId].channelFreq[chanId] == 0U)
          { 
            chanPwmTimActionTable[deviceId][chanId] = pwmTimTickCnt + ISO8200AQ_PWM_TIMER_FREQ;
            forceResync = TRUE;
          }                                       
        }
      }
      status = TRUE;
    }
  }
  return (status);
}

/******************************************************//**
 * @brief  Set Channel frequency
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @param[in] chanId (from 0 to ISO8200AQ_NB_CHANNELS-1)
 * @param[in] newFreq new frequency in 1/10Hz (from 0 to 1000)
 * @retval TRUE if successfull, FALSE if failure
 **********************************************************/
bool Iso8200aq_SetChannelFreq(uint8_t deviceId, uint8_t chanId, uint16_t newFreq)
{
  bool status = FALSE;
  if (deviceId <= (iso8200aqDriverInstance - 1U ))
  {
    if (chanId < ISO8200AQ_NB_CHANNELS)
    {
      if (newFreq > ISO8200AQ_MAX_CHAN_FREQ)
      {
         newFreq = ISO8200AQ_MAX_CHAN_FREQ;
      }
      if (driverPrm.device[deviceId].channelFreq[chanId] != newFreq)
      {  
        if (newFreq != 0U)
        {
          uint32_t period = (uint32_t)ISO8200AQ_PWM_TIMER_FREQ * 10U / newFreq;
          chanPwmTimPeriodHigh[deviceId][chanId] = period * driverPrm.device[deviceId].channelDc[chanId] / 100U;
          chanPwmTimPeriodLow[deviceId][chanId] = period - chanPwmTimPeriodHigh[deviceId][chanId];
          /* If old frequency was 0, force recynchronisation after 1 period delay*/
          if (driverPrm.device[deviceId].channelFreq[chanId] == 0U)
          { 
            chanPwmTimActionTable[deviceId][chanId] = pwmTimTickCnt + period;
            forceResync = TRUE;
          }                  
        }
        else
        {
          if (driverPrm.device[deviceId].channelDc[chanId] == 100U)
          {
            chanPwmTimPeriodHigh[deviceId][chanId] = 0xFFFFFFFFU;
          }
          else
          {
            chanPwmTimPeriodHigh[deviceId][chanId] = 0;
          }  
          chanPwmTimPeriodLow[deviceId][chanId] = 0;
        }
        /* Update frequency */
        driverPrm.device[deviceId].channelFreq[chanId] = newFreq;   
      }
      status = TRUE;
    }
  }
  return (status);
}

/******************************************************//**
 * @brief  Sets the number of devices to be used 
 * @param[in] nbDevices (from 1 to MAX_NUMBER_OF_DEVICES)
 * @retval TRUE if successfull, FALSE if failure, attempt to set a number of 
 * devices greater than MAX_NUMBER_OF_DEVICES
 **********************************************************/
bool Iso8200aq_SetNbDevices(uint8_t nbDevices)
{
  bool status = FALSE; 
  if (nbDevices <= MAX_NUMBER_OF_DEVICES)
  {
    iso8200aqDriverInstance = 0;
    numberOfDevices = nbDevices;
    status = TRUE;
  }
  return (status);
}

/******************************************************//**
 * @brief  Enable or disable one channel of a specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @param[in] chanId Channel to update (from 0 to ISO8200AQ_NB_CHANNELS-1)
 * @param[in] chanStatus New status of the channel
 * @retval TRUE if channels status update was done
 **********************************************************/
bool Iso8200aq_SetOneChannelStatus(uint8_t deviceId, uint8_t chanId, uint8_t chanStatus)
{
  bool status = FALSE; 
  if (deviceId <= (iso8200aqDriverInstance - 1U))
  {
    if (chanId < ISO8200AQ_NB_CHANNELS)
    {
      /* If the PWM mode is enabled, only force the DC of the related channel */
      if (iso8200aqPwmEnable != 0U)
      {
        if (chanStatus != 0U)
        {
          status = Iso8200aq_SetChannelDc(deviceId, chanId, 100U);  
        }
        else
        {
          status = Iso8200aq_SetChannelDc(deviceId, chanId, 0U);  
        }
      }
      else
      {
        uint8_t chanBitmap = driverPrm.device[deviceId].chanEnBitmap;
        
        if (chanStatus != 0U)
        {
          chanBitmap |= (0x1U << chanId);
        }
        else
        {
          chanBitmap &= ~(0x1U << chanId)  ;
        }
        
        status = Iso8200aq_SetAllChannelStatus(deviceId, chanBitmap);
      }
    }
  }
  return (status);
}

/******************************************************//**
 * @brief Enable outputs of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @param[in] enable 1 to enable, 0 to disable
 * @retval TRUE if successfull, FALSE if failure
 **********************************************************/
bool Iso8200aq_SetOutEnable(uint8_t deviceId, uint8_t enable)
{
  bool status = FALSE; 
  if (deviceId <= (iso8200aqDriverInstance - 1U))
  {
    if (enable != 0U)
    {
      Iso8200aq_Board_OutEnable(deviceId);
    }
    else
    {
      Iso8200aq_Board_OutDisable(deviceId);
    }
    status = TRUE;
  }
  return (status);
}

/******************************************************//**
 * @brief  Enable or disable PWM 
 * @param[in] enable (1 to enable, 0 to disable)
 * @retval TRUE if successfull, FALSE if failure
 **********************************************************/
bool Iso8200aq_SetPwmEnable(uint8_t enable)
{
  /* Disable PWM timer to prevent concurrent access via the tick update */
  iso8200aqPwmEnable = 0;
 
  if (enable != 0U)
  {
    int8_t deviceId, chanId;

    /* Clear queued channel bitmap */
    queuedBitmap = FALSE;
    
    /* Reset channel context */
    for (deviceId = (int8_t)iso8200aqDriverInstance - 1; deviceId >= 0; deviceId--)
    {
      for (chanId = (int8_t)ISO8200AQ_NB_CHANNELS - 1; chanId >= 0; chanId--)
      {
        chanPwmTimActionTable[deviceId][chanId] = 0;
      }
      driverPrm.device[deviceId].chanEnBitmap = 0;
    }
  }
  else
  {
    int8_t deviceId, chanId;
    /* Disable all channels */
    for (deviceId = (int8_t)iso8200aqDriverInstance - 1; deviceId >= 0; deviceId--)
    {
      for (chanId = (int8_t)ISO8200AQ_NB_CHANNELS - 1; chanId >= 0; chanId--)
      { 
        if (Iso8200aq_QueueChannelBitmap((uint8_t)deviceId, 0U) == 0)
        {
          Iso8200aq_ErrorHandler(ISO8200AQ_ERROR_4);
        }
      }
    }
    Iso8200aq_SendQueuedChannelBitmap();
  }

  pwmTimNextAction = 0;
  pwmTimTickCnt = 0;
  iso8200aqPwmEnable = enable;
  
  return (TRUE);
}

/******************************************************//**
 * @brief  Set the SPI frequency 
 * @retval SPI frequency in Hz
 * @retval TRUE if successfull, FALSE if failure
 **********************************************************/ 
bool Iso8200aq_SetSpiFreq(uint32_t newSpiFreq)
{
  bool status = FALSE; 
  
  if(Iso8200aq_Board_SpiInit(newSpiFreq) == 0U)
  {
    driverPrm.spiFreq = newSpiFreq;
    status = TRUE;  
  } 

  return (status);
}

/******************************************************//**
 * @brief  Get timing in us
 * @param[in] timingId  0 for Tc(SS)
 * @param[in] timingValue in us
 * @retval TRUE if successfull, FALSE if failure
 **********************************************************/
bool Iso8200aq_SetTiming(uint8_t timingId, uint8_t timingValue)
{
  bool status = FALSE; 
  if (timingId == 0U)
  {
    driverPrm.timingTcss = timingValue;
    status = TRUE;
  }
  return (status);
}


/**
  * @}
  */


/** @addtogroup ISO8200AQ_Private_functions
  * @{
  */  

/******************************************************//**
 * @brief Error handler which calls the user callback (if defined)
 * @param[in] error Number of the error
 * @retval None
 **********************************************************/
void Iso8200aq_ErrorHandler(uint16_t error)
{
  if (pErrorHandlerCallback != (void *)0)
  {
    (void) pErrorHandlerCallback(error);
  }
  else   
  {
    while(TRUE)
    {
      /* Infinite loop */
    }
  }
}

/******************************************************//**
 * @brief  Handlers of the fault interrupt which calls 
 * the user callback (if defined)
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 **********************************************************/
void Iso8200aq_FaultInterruptHandler(uint8_t deviceId)
{
  if (pFaultInterruptCallback != (void *)0)
  {
    /* Set isr flag */
    isrFlag = TRUE;
    
    pFaultInterruptCallback(deviceId);
    
    /* Reset isr flag */
    isrFlag = FALSE;   
  }
}

/******************************************************//**
 * @brief  Guard timer tick
 * @retval None
 **********************************************************/
void Iso8200aq_GuardTimerTick(void)
{
  if (spiLockTime > 0)
  {  
    spiLockTime -= (int16_t)ISO8200AQ_GUARD_TIMER_TICK_IN_US;
  }
  else
  {
    Iso8200aq_Board_GuardTimerStop(); 
  }
}

/******************************************************//**
 * @brief  Handlers of the PGood interrupt which calls 
 * the user callback (if defined)
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 **********************************************************/
void Iso8200aq_PGoodInterruptHandler(uint8_t deviceId)
{
  if (pPGoodInterruptCallback != (void *)0)
  {
    /* Set isr flag */
    isrFlag = TRUE;
    
    pPGoodInterruptCallback(deviceId);
    
    /* Reset isr flag */
    isrFlag = FALSE;   
  }
}

/******************************************************//**
 * @brief  PWM timer tick
 * @retval None
 **********************************************************/
void Iso8200aq_PwmTimerTick(void)
{
  if (iso8200aqPwmEnable != 0U)
  {
    /* Set isr flag */
    isrFlag = TRUE;    
    
    /*  Update channels first to minimize latency */
    Iso8200aq_SendQueuedChannelBitmap();
    
    /* Then prepare next tick updates */
    if ((pwmTimTickCnt == pwmTimNextAction)||(forceResync == FALSE))
    {
      uint32_t delayToNextAction = 0xFFFFFFFFU;
      uint32_t tmpDelay;
      int8_t deviceId, chanId;
      uint8_t newChanBitMap;
      
      forceResync = FALSE;
      for (deviceId = (int8_t)(iso8200aqDriverInstance) - 1; deviceId >= 0; deviceId--)
      {
        /*  Load the current bitmap of the corresponding device */
        newChanBitMap = driverPrm.device[deviceId].chanEnBitmap;

        for (chanId = (int8_t)(ISO8200AQ_NB_CHANNELS - 1U); chanId >= 0; chanId--)
        {
          /* check if the channel has to be updated */
          if (pwmTimTickCnt == chanPwmTimActionTable[deviceId][chanId])
          {
            if (chanPwmTimPeriodHigh[deviceId][chanId] == 0U)
            {
              /* clear the channel */
              newChanBitMap &= ~(0x1U <<(uint8_t)chanId); 
            }
            else if (chanPwmTimPeriodLow[deviceId][chanId] == 0U)
            {
              /* Set the channel */
              newChanBitMap |= (0x1U <<(uint8_t)chanId); 
            }
            else
            {
              /* Toggle the channel */
              newChanBitMap ^= (0x1U <<(uint8_t)chanId); 
            }
            /* Compute the tick of the next action on this channel */
            if ((newChanBitMap & (0x1U <<(uint8_t)chanId)) != 0U)
            {
              chanPwmTimActionTable[deviceId][chanId] += chanPwmTimPeriodHigh[deviceId][chanId];
            }
            else
            {
              chanPwmTimActionTable[deviceId][chanId] += chanPwmTimPeriodLow[deviceId][chanId];
            }
          }
          
          if (pwmTimTickCnt < chanPwmTimActionTable[deviceId][chanId])
          {
            tmpDelay =  chanPwmTimActionTable[deviceId][chanId] - pwmTimTickCnt;
          }
          else
          {
           tmpDelay =  0xFFFFFFFFU - pwmTimTickCnt + chanPwmTimActionTable[deviceId][chanId]; 
          }
            
          if (tmpDelay < delayToNextAction)
          {
            delayToNextAction = tmpDelay;
            pwmTimNextAction = chanPwmTimActionTable[deviceId][chanId];
          }        
        }
        if (Iso8200aq_QueueChannelBitmap((uint8_t)deviceId, newChanBitMap) == 0)
        {
          Iso8200aq_ErrorHandler(ISO8200AQ_ERROR_5);  
        }
      }
    }
    /*  Increment tick */
    pwmTimTickCnt++;
    
    /* Reset isr flag */
    isrFlag = FALSE;    
  }
}
/******************************************************//**
 * @brief  Set the parameters of the device to values of pInitPrm structure
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @param pInitPrm pointer to a structure containing the initial device parameters 
 * @retval None
 **********************************************************/
void Iso8200aq_SetDeviceParamsToGivenValues(uint8_t deviceId, iso8200aq_Init_t *pInitPrm)
{
  int8_t loop;
  driverPrm.timingTcss = pInitPrm->timingTcss;
  driverPrm.spiFreq = pInitPrm->spiFreq;
  driverPrm.device[deviceId].chanEnBitmap = pInitPrm->chanEnBitmap;
  for (loop = (int8_t)ISO8200AQ_NB_CHANNELS - 1; loop >= 0; loop--)
  {
    driverPrm.device[deviceId].channelFreq[loop] = 0;
    driverPrm.device[deviceId].channelDc[loop] = 0;
  }
}

/******************************************************//**
 * @brief  Sets the parameters of the device to predefined values 
 * from iso8200aq_target_config.h
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 **********************************************************/
void Iso8200aq_SetDeviceParamsToPredefinedValues(uint8_t deviceId)
{
  int8_t loop;
  driverPrm.timingTcss = ISO8200AQ_CONF_PARAM_TIMING_TCSS;
  driverPrm.spiFreq = ISO8200AQ_CONF_PARAM_SPI_FREQ;
  switch (deviceId)
  {
    case 1:
      driverPrm.device[deviceId].chanEnBitmap = ISO8200AQ_CONF_PARAM_CHANNEL_ENABLE_BITMAP_DEVICE_1;
      break;
    case 0:
    default:
      driverPrm.device[deviceId].chanEnBitmap = ISO8200AQ_CONF_PARAM_CHANNEL_ENABLE_BITMAP_DEVICE_0;
      break;      
  }
  for (loop = (int8_t)ISO8200AQ_NB_CHANNELS - 1; loop >= 0; loop--)
  {
    driverPrm.device[deviceId].channelFreq[loop] = 0;
    driverPrm.device[deviceId].channelDc[loop] = 0;
  }
}

/******************************************************//**
 * @brief  Write and receive a byte via SPI
 * @param[in] pByteToTransmit pointer to the byte to transmit
 * @param[in] pReceivedByte pointer to the received byte
 * @retval None
 **********************************************************/
void Iso8200aq_WriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte)
{
  if (Iso8200aq_Board_SpiWriteBytes(pByteToTransmit, pReceivedByte, numberOfDevices) != 0U)
  {
    Iso8200aq_ErrorHandler(ISO8200AQ_ERROR_6);
  }
  else
  {
    spiLockTime = (int16_t)driverPrm.timingTcss - (int16_t)ISO8200AQ_GUARD_TIMER_TICK_IN_US;
    Iso8200aq_Board_GuardTimerStart();
  }
  queuedBitmap = FALSE;
  if (isrFlag)
  {
    spiPreemtionByIsr = TRUE;
  }

}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
