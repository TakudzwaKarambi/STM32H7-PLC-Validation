#ifndef CLT01_38SQ7_CFG_H
#define CLT01_38SQ7_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * CLT01-38SQ7 Driver Configuration
 *
 * This file contains all compile-time configuration options for the
 * CLT01-38SQ7 digital input driver.
 *
 * Modify this file only when adapting the driver to a different hardware
 * platform or changing driver behaviour.
 *===========================================================================*/


/*=============================================================================
 * Driver Version
 *===========================================================================*/
#define CLT01_DRIVER_VERSION_MAJOR        (1U)
#define CLT01_DRIVER_VERSION_MINOR        (0U)
#define CLT01_DRIVER_VERSION_PATCH        (0U)


/*=============================================================================
 * SPI Configuration
 *===========================================================================*/

/*
 * Select SPI frame mode.
 *
 * 0 = 8-bit SPI mode  (SPM pin tied HIGH)
 * 1 = 16-bit SPI mode (SPM pin tied LOW)
 *
 * Your PCB currently operates in 16-bit mode.
 */
#define CLT01_SPI_16BIT_MODE              (1U)


/*
 * SPI timeout in milliseconds.
 */
#define CLT01_SPI_TIMEOUT_MS              (100U)


/*=============================================================================
 * Device Timing
 *===========================================================================*/

/*
 * Delay after asserting DI_EN before the first SPI transaction.
 */
#define CLT01_POWERUP_DELAY_MS            (10U)


/*=============================================================================
 * Diagnostics
 *===========================================================================*/

/*
 * Enable parity verification.
 */
#define CLT01_ENABLE_PARITY_CHECK         (1U)


/*
 * Enable stop-bit verification.
 */
#define CLT01_ENABLE_STOP_BIT_CHECK       (1U)


/*
 * Read GOOD pin if available.
 */
#define CLT01_ENABLE_GOOD_PIN             (1U)


/*
 * Read FAULT pin if available.
 */
#define CLT01_ENABLE_FAULT_PIN            (1U)


/*=============================================================================
 * Debug Support
 *===========================================================================*/

/*
 * Store the raw SPI frame.
 *
 * Highly recommended during board bring-up.
 */
#define CLT01_STORE_RAW_FRAME             (1U)


/*
 * Decode individual input channels.
 */
#define CLT01_DECODE_CHANNELS             (1U)


/*=============================================================================
 * Defensive Programming
 *===========================================================================*/

/*
 * Perform NULL pointer checks.
 */
#define CLT01_ENABLE_PARAMETER_CHECKS     (1U)


/*
 * Clear the status structure before decoding.
 */
#define CLT01_CLEAR_STATUS_BEFORE_READ    (1U)


/*=============================================================================
 * Reserved
 *===========================================================================*/

/*
 * Reserved for future expansion.
 */
#define CLT01_RESERVED                    (0U)


#ifdef __cplusplus
}
#endif

#endif /* CLT01_38SQ7_CFG_H */
