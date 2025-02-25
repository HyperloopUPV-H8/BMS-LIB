cmake_minimum_required (VERSION 3.5)

project (BMS-LIB ASM C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(STLIB_DIR ${CMAKE_CURRENT_LIST_DIR}/deps/ST-LIB)

option(USE_ETHERNET "Enable ethernet peripheral" OFF)
option(TARGET_NUCLEO "Targets the STM32H723 Nucleo development board" OFF)

message("About to configure ST-LIB for BMS-LIB")
if(NOT TARGET st-lib)
  add_subdirectory(${STLIB_DIR})
endif()
message("ST-LIB configured for BMS-LIB")

file(GLOB_RECURSE SOURCE_C ${CMAKE_CURRENT_SOURCE_DIR}/Src/*.c)
file(GLOB_RECURSE SOURCE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp)

add_library(bms-lib STATIC
  ${SOURCE_C}
  ${SOURCE_CPP}
)

target_link_libraries(bms-lib PRIVATE st-lib)

target_compile_definitions(bms-lib PRIVATE
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:USE_HAL_DRIVER>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:STM32H723xx>
  DATA_IN_D2_SRAM

  $<$<NOT:$<BOOL:${CMAKE_CROSSCOMPILING}>>:SIM_ON>

  $<$<BOOL:${USE_ETHERNET}>:STLIB_ETH>
  $<IF:$<BOOL:${TARGET_NUCLEO}>,NUCLEO,BOARD>
  $<IF:$<BOOL:${TARGET_NUCLEO}>,HSE_VALUE=8000000,HSE_VALUE=25000000>
)

target_compile_options(bms-lib PRIVATE
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mcpu=cortex-m7>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mfpu=fpv5-d16>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mfloat-abi=hard>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mthumb>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-specs=nosys.specs>
  -ffunction-sections
  -fdata-sections
  -fno-exceptions
  -fno-use-cxa-atexit
  ## disable Warnings for C files, this is Driver code from ST
  $<$<COMPILE_LANGUAGE:C>:-w>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  $<$<COMPILE_LANGUAGE:CXX>:-Wall>
  $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
  # $<$<COMPILE_LANGUAGE:CXX>:-Werror>
  -Wno-pointer-arith
)

target_include_directories(bms-lib PRIVATE
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/CMSIS/Include>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/system>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/netif/ppp>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip/apps>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip/priv>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip/prot>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/netif>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/net>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/sys>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/stdc>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/system/arch>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/BSP/Components>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/BSP/Components/lan8742>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/LWIP/App>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/LWIP/Target>
  ${STLIB_DIR}/Inc
  ${STLIB_DIR}/Inc/ST-LIB_LOW
  ${STLIB_DIR}/Inc/ST-LIB_HIGH
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/Inc/Models
)

if (PROJECT_IS_TOP_LEVEL)
    # Create symlink to compile_commands.json for IDE to pick it up
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
endif()