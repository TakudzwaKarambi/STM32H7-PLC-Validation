# PID30D Industrial PLC Firmware
### STM32H730-Based OpenPLC Compatible Industrial Controller

![Platform](https://img.shields.io/badge/MCU-STM32H730ZBT6-blue)
![Language](https://img.shields.io/badge/Language-C-green)
![IDE](https://img.shields.io/badge/IDE-STM32CubeIDE-orange)
![License](https://img.shields.io/badge/License-MIT-success)
![Status](https://img.shields.io/badge/Status-In%20Development-yellow)

---

## Overview

The AP30D Industrial PLC Firmware is a custom industrial Programmable Logic Controller (PLC) firmware developed around the **STM32H730ZBT6** microcontroller.

Unlike evaluation-board demonstrations, this project targets a **production-ready custom PCB** designed for industrial automation applications including:

- Process Control
- Chemical Automation
- HPLC Systems
- Laboratory Instrumentation
- Industrial I/O Control
- OpenPLC Runtime Integration

The firmware follows a modular architecture that separates hardware drivers, board support, process image management and PLC scan execution to closely resemble commercial PLC systems.

---

# Features

## Digital Inputs

- 8 × 24V Industrial Digital Inputs
- CLT01-38SQ7 Current Limiter Interface
- SPI Communication
- Input diagnostics

---

## Digital Outputs

- 8 × High-Side Outputs
- ISO8200AQ Driver
- Short circuit protection
- Open load detection
- Thermal shutdown diagnostics

---

## Analog Inputs

- ADS8688
- 8 Channel
- 16-bit ADC
- Bipolar Input Support
- Industrial Voltage Measurement

---

## Analog Outputs

- DAC81408
- 8 Channel
- High Resolution DAC
- Industrial Analog Outputs

---

## Ethernet

- LAN8742A PHY
- RMII Interface
- 10/100 Mbps
- OpenPLC Communication
- Modbus TCP (planned)

---

## PLC Runtime

- OpenPLC Runtime Integration
- IEC 61131-3 Compatible
- Ladder Logic Execution
- Deterministic Scan Cycle
- Process Image Based Execution

---

# Hardware

MCU

```
STM32H730ZBT6
```

Industrial Peripherals

```
ADS8688
DAC81408
ISO8200AQ
CLT01-38SQ7
LAN8742A
```

---

# Software Architecture

```
                +-----------------------+
                |    OpenPLC Runtime    |
                +-----------+-----------+
                            |
                     Process Image
                            |
                +-----------+-----------+
                |       BSP_PLC         |
                +-----------+-----------+
                            |
          +-----------------+-----------------+
          |        Hardware Drivers           |
          +-----------------+-----------------+
          | CLT | ISO | ADS | DAC | Ethernet  |
          +-----------------+-----------------+
                            |
                        STM32 HAL
                            |
                      STM32H730 MCU
```

---

# Repository Structure

```
Core/
│
├── Inc/
│
├── Src/
│
├── Drivers/
│
├── BSP/
│   ├── bsp_plc.c
│   ├── bsp_plc.h
│
├── ProcessImage/
│   ├── process_image.c
│   ├── process_image.h
│
├── Drivers/
│   ├── analog_input/
│   ├── analog_output/
│   ├── digital_input/
│   ├── digital_output/
│   └── ethernet/
│
└── Middleware/
```

---

# Current Progress

## Completed

- STM32H730 Board Bring-up
- GPIO Validation
- SPI Driver Framework
- CLT01 Driver
- ISO8200AQ Driver
- ADS8688 Driver
- DAC81408 Driver
- BSP Layer
- Process Image Framework
- Modular Driver Architecture

---

## In Progress

- LAN8742A Ethernet Bring-up
- PHY Validation
- RMII Interface Validation
- OpenPLC Runtime Integration
- PLC Scan Engine

---

## Planned

- Modbus TCP
- Modbus RTU
- Web Configuration
- SD Card Logging
- Firmware Update
- Diagnostics Dashboard

---

# Development Tools

- STM32CubeIDE
- STM32CubeMX
- STM32 HAL
- OpenPLC Editor
- OpenPLC Runtime
- Wireshark
- Logic Analyzer
- Analog Discovery 2

---

# Build

Clone the repository

```bash
git clone https://github.com/<username>/AP30D-Industrial-PLC.git
```

Open

```
STM32CubeIDE
```

Import

```
Existing STM32 Project
```

Build

```
Ctrl + B
```

Flash

```
Run → Debug
```

---

# Project Goals

The objective of this project is to develop a fully functional industrial PLC platform capable of running OpenPLC applications on custom hardware while supporting industrial field interfaces including:

- Digital I/O
- Analog I/O
- Ethernet
- Modbus
- Industrial Sensors
- Process Automation

---

# Author

**Takudzwa Karambi**

Embedded Systems Engineer

Industrial Automation | PLC Development | STM32 | Process Control

---

# Acknowledgements

- STMicroelectronics
- OpenPLC Project
- LAN8742A Documentation
- ADS8688 Datasheet
- DAC81408 Datasheet
- ISO8200AQ Documentation
- CLT01-38SQ7 Documentation

---

# License

This project is released under the MIT License.
