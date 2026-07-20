#ifndef ISO8200AQ_TARGET_CONFIG_H
#define ISO8200AQ_TARGET_CONFIG_H

#define MAX_NUMBER_OF_DEVICES                         (1U)

// Configuration for Device 0 (Your active chip)
#define ISO8200AQ_CONF_PARAM_CHANNEL_ENABLE_BITMAP_DEVICE_0 (0x00U)

// Dummy definitions to satisfy the multi-device initialization loop in the vendor driver
#define ISO8200AQ_CONF_PARAM_CHANNEL_ENABLE_BITMAP_DEVICE_1 (0x00U)
#define ISO8200AQ_CONF_PARAM_CHANNEL_ENABLE_BITMAP_DEVICE_2 (0x00U)
#define ISO8200AQ_CONF_PARAM_CHANNEL_ENABLE_BITMAP_DEVICE_3 (0x00U)

/* Timings and frequencies matching your SPI configuration */
#define ISO8200AQ_CONF_PARAM_SPI_FREQ                 (1000000U)    // 1 MHz SPI clock limit
#define ISO8200AQ_CONF_PARAM_TIMING_TCSS              (1U)          // Setup delay in us

#endif /* ISO8200AQ_TARGET_CONFIG_H */
