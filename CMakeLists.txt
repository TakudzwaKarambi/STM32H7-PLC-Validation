cmake_minimum_required(VERSION 3.20)

# Set Cross-Compilation system target BEFORE project()
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Prevent CMake from trying to compile a test program with host compiler
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Project Name matching STM32CubeIDE project
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
    -DSTM32H730xx          # Matching your target MCU
    -DUSE_HAL_DRIVER
)

add_link_options(
    ${MCU_FLAGS}
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
    --specs=nano.specs
    --specs=nosys.specs
)

# ==============================================================================
# 2. Source Files
# ==============================================================================
# Collect C source files
file(GLOB_RECURSE CORE_SOURCES "Core/*.c")
file(GLOB_RECURSE DRIVER_SOURCES "Drivers/*.c")

# Startup Assembly File (Matches Core/Startup/ folder)
file(GLOB_RECURSE STARTUP_ASM "Core/Startup/*.s")

# Target Executable
add_executable(${PROJECT_NAME}.elf 
    ${CORE_SOURCES} 
    ${DRIVER_SOURCES} 
    ${STARTUP_ASM}
)

# ==============================================================================
# 3. Header Include Paths (Scoped to Target)
# ==============================================================================
target_include_directories(${PROJECT_NAME}.elf PRIVATE
    # Core Headers
    Core/Inc
    Core/Inc/analog_subsystem
    Core/Inc/BSP_subsystem
    Core/Inc/digital_subsystem

    # Hardware Drivers & Middleware (From your git status structure)
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
# 4. Linker Script & Post-Build Actions
# ==============================================================================
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32H730ZBTX_FLASH.ld)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating .hex/.bin outputs and calculating footprint..."
)
