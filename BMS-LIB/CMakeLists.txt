cmake_minimum_required (VERSION 3.4)
project (bms-lib ASM C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(STLIB_DIR "/opt/ST-LIB")
set(BMS-LIB bms-lib)

file(GLOB_RECURSE SOURCE_C ${CMAKE_SOURCE_DIR}/ *.c)
file(GLOB_RECURSE SOURCE_CPP ${CMAKE_SOURCE_DIR}/ *.cpp)
file(GLOB_RECURSE SOURCE_H ${CMAKE_SOURCE_DIR}/ *.h)
file(GLOB_RECURSE SOURCE_HPP ${CMAKE_SOURCE_DIR}/ *.hpp)

add_library(${BMS-LIB} STATIC
  ${SOURCE_C}
  ${SOURCE_CPP}
  ${SOURCE_H}
  ${SOURCE_CPP}
)

target_link_libraries(${BMS-LIB} st-lib)

target_compile_definitions(${BMS-LIB} PUBLIC
  -DUSE_HAL_DRIVER
  -DSTM32H723xx
  -DDATA_IN_D2_SRAM
)

target_include_directories(${BMS-LIB} PUBLIC
  ${STLIB_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
  ${STLIB_DIR}/Drivers/CMSIS/Include
  ${STLIB_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc
  ${STLIB_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
  ${STLIB_DIR}/Inc
  ${STLIB_DIR}/Inc/HALAL/Models
  ${STLIB_DIR}/Inc/HALAL/Services
  ${STLIB_DIR}/Inc/ST-LIB_LOW
  ${STLIB_DIR}/Inc/ST-LIB_HIGH
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/system
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/netif/ppp
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip/apps
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip/priv
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip/prot
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/netif
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/net
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/sys
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/stdc
  ${STLIB_DIR}/Middlewares/Third_Party/LwIP/system/arch
  ${STLIB_DIR}/Middlewares/Third_Party/
  ${STLIB_DIR}/Drivers/BSP/Components
  ${STLIB_DIR}/Drivers/BSP/Components/lan8742
  ${STLIB_DIR}/LWIP/App
  ${STLIB_DIR}/LWIP/Target
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/Inc/Models
)


target_compile_options(${BMS-LIB} PUBLIC
  -mcpu=cortex-m7
  -mfpu=fpv5-d16
  -mfloat-abi=hard
  -mthumb
  
  -O0
  -g3
  -ffunction-sections
  -fdata-sections
  -fno-exceptions
  -fno-use-cxa-atexit
  -fno-use-cxa-atexit

  -Wall
  -Wno-pointer-arith

  --specs=nosys.specs
)
