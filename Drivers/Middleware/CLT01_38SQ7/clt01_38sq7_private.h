#ifndef CLT01_38SQ7_PRIVATE_H
#define CLT01_38SQ7_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "clt01_38sq7.h"
#include "clt01_38sq7_cfg.h"

/*=============================================================================
 * CLT01-38SQ7 SPI Frame Definition (16-bit Mode)
 *
 *  Bit15  IN8
 *  Bit14  IN7
 *  Bit13  IN6
 *  Bit12  IN5
 *  Bit11  IN4
 *  Bit10  IN3
 *  Bit9   IN2
 *  Bit8   IN1
 *  Bit7   /UVA
 *  Bit6   /OTA
 *  Bit5   PC1
 *  Bit4   PC2
 *  Bit3   PC3
 *  Bit2   PC4
 *  Bit1   STOP_L
 *  Bit0   STOP_H
 *===========================================================================*/

/*=============================================================================
 * Bit Masks
 *===========================================================================*/

#define CLT_INPUT_MASK              (0xFF00U)

#define CLT_UVA_MASK                (0x0080U)
#define CLT_OTA_MASK                (0x0040U)

#define CLT_PC1_MASK                (0x0020U)
#define CLT_PC2_MASK                (0x0010U)
#define CLT_PC3_MASK                (0x0008U)
#define CLT_PC4_MASK                (0x0004U)

#define CLT_STOP_LOW_MASK           (0x0002U)
#define CLT_STOP_HIGH_MASK          (0x0001U)

/*=============================================================================
 * Bit Positions
 *===========================================================================*/

#define CLT_INPUT_SHIFT             (8U)

#define CLT_UVA_BIT                 (7U)
#define CLT_OTA_BIT                 (6U)

#define CLT_PC1_BIT                 (5U)
#define CLT_PC2_BIT                 (4U)
#define CLT_PC3_BIT                 (3U)
#define CLT_PC4_BIT                 (2U)

#define CLT_STOP_LOW_BIT            (1U)
#define CLT_STOP_HIGH_BIT           (0U)

/*=============================================================================
 * Internal SPI Helpers
 *===========================================================================*/

/**
 * @brief Assert Chip Select.
 */
void CLT_CS_Low(CLT01_Handle_t *hclt);

/**
 * @brief Deassert Chip Select.
 */
void CLT_CS_High(CLT01_Handle_t *hclt);

/**
 * @brief Read one complete SPI frame.
 */
HAL_StatusTypeDef CLT_SPI_ReadFrame(
        CLT01_Handle_t *hclt,
        uint16_t *frame);
static inline bool CLT_GetInputBit(uint16_t frame, uint8_t channel);
/*=============================================================================
 * Internal Decoder Helpers
 *===========================================================================*/

/**
 * @brief Decode all eight digital inputs.
 */
void CLT_DecodeInputs(
        uint16_t frame,
        CLT01_Status_t *status);

/**
 * @brief Decode UVA/OTA diagnostics.
 */
void CLT_DecodeDiagnostics(
        uint16_t frame,
        CLT01_Status_t *status);

/**
 * @brief Validate parity bits.
 */
bool CLT_CheckParity(uint16_t frame);

/**
 * @brief Validate stop bits.
 */
bool CLT_CheckStopBits(uint16_t frame);

/**
 * @brief Update overall frame validity.
 */
void CLT_UpdateFrameStatus(
        CLT01_Status_t *status);

/**
 * @brief Read optional GOOD and FAULT GPIO pins.
 */
void CLT_UpdateHardwareStatus(
        CLT01_Handle_t *hclt,
        CLT01_Status_t *status);

#ifdef __cplusplus
}
#endif

#endif /* CLT01_38SQ7_PRIVATE_H */
