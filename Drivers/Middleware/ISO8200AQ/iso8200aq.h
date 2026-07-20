/**
  ******************************************************************************
  * @file    iso8200aq.h 
  * @author  IPC Rennes
  * @version V1.0.0
  * @date    March 21th, 2017
  * @brief   Header for ISO8200AQ driver (galvanic isolated 8 channels switch driver)
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ISO8200AQ_H
#define ISO8200AQ_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "iso8200aq_target_config.h"

   
/** @addtogroup BSP
  * @{
  */   
   

/** @addtogroup ISO8200AQ
  * @{
  */   
   
/* Exported Constants --------------------------------------------------------*/

/** @defgroup ISO8200AQ_Exported_Constants ISO8200AQ Exported Constants 
  * @{
  */   
/** Current FW major version */
#define ISO8200AQ_FW_MAJOR_VERSION (uint8_t)(1)
/** Current FW minor version */
#define ISO8200AQ_FW_MINOR_VERSION (uint8_t)(0)
/** Current FW patch version */
#define ISO8200AQ_FW_PATCH_VERSION (uint8_t)(0)
/** Current FW version */
#define ISO8200AQ_FW_VERSION       (uint32_t)((ISO8200AQ_FW_MAJOR_VERSION<<16)|\
                                              (ISO8200AQ_FW_MINOR_VERSION<<8)|\
                                              (ISO8200AQ_FW_PATCH_VERSION))

/** Number of channels by relay ISO8200AQ */
#define ISO8200AQ_NB_CHANNELS   (8U)

  /**
  * @}
  */

/* Exported Types  -------------------------------------------------------*/

/** @defgroup ISO8200AQ_Exported_Types ISO8200AQ Exported Types
  * @{
  */   

/** @defgroup ISO8200AQ_Initialization_Structure ISO8200AQ Initialization Structure
  * @{
  */
/** Relay driver initialization structure definition   */

typedef struct
{
  /**Initial SPI frequency in Hz */
  uint32_t spiFreq;   
  /**Initial Tcss delay in us */
  uint8_t timingTcss;  
  /**Initial channel enable bitmap */
  uint8_t chanEnBitmap;  
} iso8200aq_Init_t;

/**
  * @}
  */

/** @defgroup Device_Parameters Device Parameters
  * @{
  */

/** Device Parameters Structure Type */
typedef struct {
  /**channel enable output bitmap */
  uint8_t chanEnBitmap;                        
  /**channel duty cycle table  */
  uint8_t channelDc[ISO8200AQ_NB_CHANNELS];    
  /**channel frequency table */
  uint16_t channelFreq[ISO8200AQ_NB_CHANNELS]; 
}deviceParams_t; 

/** Driver Parameters Structure Type */
typedef struct {
  /**Tcss delay in us */
  uint8_t timingTcss;                           
  /**SPI frequency in Hz */
  uint32_t spiFreq;                             
  /**Device parameters array */
  deviceParams_t device[MAX_NUMBER_OF_DEVICES]; 
}driverParams_t; 
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/


/** @defgroup ISO8200AQ_Exported_Functions ISO8200AQ Exported Functions
  * @{
  */   

/** @defgroup ISO8200AQ_Library_Functions ISO8200AQ Library Functions
  * @{
  */   
void Iso8200aq_AttachErrorHandler(void (*callback)(uint16_t));  /* Attach a user callback to the error handler */
void Iso8200aq_AttachFaultInterrupt(void (*callback)(uint8_t));    /* Attach a user callback to the Fault Interrupt */
void Iso8200aq_AttachPGoodInterrupt(void (*callback)(uint8_t));    /* Attach a user callback to the PGood Interrupt */
uint8_t Iso8200aq_GetAllChannelStatus(uint8_t deviceId);        /* Get all channels status of a given device */
uint8_t Iso8200aq_GetChannelDc(uint8_t deviceId, uint8_t chanId); /* Get channel duty cycle of a given channel for a given device */
uint16_t Iso8200aq_GetChannelFreq(uint8_t deviceId, uint8_t chanId); /* Get channel frequency of a given channel for a given device */
uint8_t Iso8200aq_GetFaultRegister(uint8_t deviceId);             /* Get Fault register    */
uint8_t Iso8200aq_GetFaultStatus(uint8_t deviceId);             /* Get Fault pin status  */
uint32_t Iso8200aq_GetFwVersion(void);                          /* Return the FW version */
uint8_t Iso8200aq_GetNbDevices(void);                           /* Return the number of initiliased devices */
uint8_t Iso8200aq_GetOneChannelStatus(uint8_t deviceId, uint8_t chanId);  /* Get all channels status of a given device */
uint8_t Iso8200aq_GetOutEnable(uint8_t deviceId);               /* Get Output Enable state */
uint8_t Iso8200aq_GetPGoodStatus(uint8_t deviceId);             /* Get PGood pin status */
uint8_t Iso8200aq_GetPwmEnable(void);                              /* Get PWM status */
//relayDrv_t* Iso8200aq_GetRelayHandle(void);                     /* Return handle of the relay driver handle */
uint32_t Iso8200aq_GetSpiFreq(void);                            /* Get Spi frequency in Hz */
uint8_t Iso8200aq_GetTiming(uint8_t timingId);                  /* Get timing */
void Iso8200aq_Init(void* pInit);                               /* Start the ISO8200AQ library */
bool Iso8200aq_QueueChannelBitmap(uint8_t deviceId,  uint8_t newChanBitmap);  /* Queued channel bitmap */
uint16_t Iso8200aq_ReadId(void);                                /* Read Id to get driver instance */
bool Iso8200aq_SendQueuedChannelBitmap(void);                   /* Simultenously send queued channel bitmaps to all devices */
bool Iso8200aq_SetAllChannelStatus(uint8_t deviceId, uint8_t newChanBitmap); /* Set all channels status of a given device */
bool Iso8200aq_SetChannelDc(uint8_t deviceId, uint8_t chanId, uint8_t newDc);/* Set the channel duty cycle of a given channel for a given device */
bool Iso8200aq_SetChannelFreq(uint8_t deviceId, uint8_t chanId, uint16_t newFreq); /* Set the channel frequency of a given channel for a given device */
bool Iso8200aq_SetNbDevices(uint8_t nbDevices);                 /* Set the number of devices */
bool Iso8200aq_SetOneChannelStatus(uint8_t deviceId, uint8_t chanId, uint8_t chanStatus); /* Set one channel status of a given device */
bool Iso8200aq_SetOutEnable(uint8_t deviceId, uint8_t enable);  /* Set Ouput Enable */
bool Iso8200aq_SetPwmEnable(uint8_t enable);                     /* Set PWM status */
bool Iso8200aq_SetSpiFreq(uint32_t newSpiFreq);                    /* Set Spi frequency in Hz */
bool Iso8200aq_SetTiming(uint8_t timingId, uint8_t timingValue);  /* Set timing */


/**
  * @}
  */

/** @defgroup ISO8200AQ_Board_Linked_Functions ISO8200AQ Board Linked Functions
  * @{
  */   
/**Delay of the requested number of milliseconds */
extern void Iso8200aq_Board_Delay(uint32_t delay);         
/**Enable Irq */
extern void Iso8200aq_Board_EnableIrq(void);               
/**Disable Irq */
extern void Iso8200aq_Board_DisableIrq(void);              
/**Get state of Fault pin */
extern uint8_t Iso8200aq_Board_GetFaultPinState(uint8_t deviceId);
/**Get state of Output Enable pin */
extern uint8_t Iso8200aq_Board_GetOutPinState(uint8_t deviceId);
/**Get state of PGood pin */
extern uint8_t Iso8200aq_Board_GetPGoodPinState(uint8_t deviceId);
/**Initialise GPIOs used for ISO8200AQs */
extern void Iso8200aq_Board_GpioInit(uint8_t deviceId);   
/**Start guard timer */
extern void Iso8200aq_Board_GuardTimerStart(void);
/**Stop guard timer */
extern void Iso8200aq_Board_GuardTimerStop(void);
/**Disable OUT_EN pin */
extern void Iso8200aq_Board_OutDisable(uint8_t deviceId);   
/**Enable OUT_EN pin */
extern void Iso8200aq_Board_OutEnable(uint8_t deviceId);
/**Initialise the SPI used for ISO8200AQs */
extern uint8_t Iso8200aq_Board_SpiInit(uint32_t spiFreq);   
/**Write bytes to the ISO8200AQs via SPI */
extern uint8_t Iso8200aq_Board_SpiWriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte, uint8_t nbDevices); 
/**Initilise the timer which is used for PWM generation */
extern uint8_t Iso8200aq_Board_TimerInit(uint32_t pwmFreq, uint32_t guardFreq); 
/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
  }
#endif

#endif /* #ifndef ISO8200AQ_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
