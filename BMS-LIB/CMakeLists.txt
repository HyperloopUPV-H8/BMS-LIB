cmake_minimum_required (VERSION 3.4)
project (bms-lib ASM C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(STLIB_DIR ../../ST-LIB)
set(BMS-LIB bms-lib)

add_subdirectory(${STLIB_DIR} ${STLIB_DIR}/build)

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
  Inc
  Inc/Models
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
