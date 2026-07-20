cmake_minimum_required(VERSION 3.20)

# Project name matching your STM32CubeIDE project name
project(FUNCTIONAL_VALIDATION C CXX ASM)

set(CMAKE_C_STANDARD 11)

# ==============================================================================
# 1. STM32H730 Hardware & Compilation Flags
# ==============================================================================
set(MCU_FLAGS
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard
)

add_compile_options(
    ${MCU_FLAGS}
    -Wall
    -Wextra
    -fdata-sections
    -ffunction-sections
    -DSTM32H730xx          # Exact symbol for STM32H730ZBTX
    -DUSE_HAL_DRIVER
)

add_link_options(
    ${MCU_FLAGS}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    --specs=nano.specs
    --specs=nosys.specs
)

# ==============================================================================
# 2. Header Include Paths (Matching your exact folder structure)
# ==============================================================================
include_directories(
    # Core Headers
    Core/Inc
    Core/Inc/analog_subsystem
    Core/Inc/BSP_subsystem
    Core/Inc/digital_subsystem

    # Hardware Drivers (Drivers/Middleware/)
    Drivers/Middleware/ADS8688
    Drivers/Middleware/CLT01_38SQ7
    Drivers/Middleware/Common
    Drivers/Middleware/DAC81408
    Drivers/Middleware/ISO8200AQ

    # HAL & CMSIS Headers
    Drivers/STM32H7xx_HAL_Driver/Inc
    Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
    Drivers/CMSIS/Device/ST/STM32H7xx/Include
    Drivers/CMSIS/Include
)

# ==============================================================================
# 3. Source Files (.c and .s)
# ==============================================================================
# Collect all .c files across Core, Drivers, and Middleware automatically
file(GLOB_RECURSE ALL_C_SOURCES
    "Core/*.c"
    "Drivers/*.c"
)

# Startup Assembly File (Matches your Startup folder)
file(GLOB STARTUP_ASM "Core/Startup/*.s")

# Combine all source files
set(ALL_SOURCES
    ${ALL_C_SOURCES}
    ${STARTUP_ASM}
)

# ==============================================================================
# 4. Linker Script & Binary Output Generation
# ==============================================================================
# Automatically locates your exact STM32H730ZBTX_FLASH.ld file in the root directory
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32H730ZBTX_FLASH.ld)

add_executable(${CMAKE_PROJECT_NAME}.elf ${ALL_SOURCES})

target_link_options(${CMAKE_PROJECT_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
)

# Post-build step: Creates .hex and .bin outputs and prints memory size
add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}.elf> ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}.elf> ${CMAKE_PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}.elf>
    COMMENT "Building binaries and calculating footprint..."
)
