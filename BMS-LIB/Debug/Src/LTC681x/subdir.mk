################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/LTC681x/LTC681x.cpp 

OBJS += \
./Src/LTC681x/LTC681x.o 

CPP_DEPS += \
./Src/LTC681x/LTC681x.d 


# Each subdirectory must supply rules for building sources it contributes
Src/LTC681x/%.o Src/LTC681x/%.su: ../Src/LTC681x/%.cpp Src/LTC681x/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -c -I"C:/Users/aleja/Proyectos/Hyper-H8/ST-LIB" -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/aleja/Proyectos/Hyper-H8/ST-LIB/Inc" -I"C:/Users/aleja/Proyectos/Hyper-H8/ST-LIB/Inc/HALAL/Models" -I"C:/Users/aleja/Proyectos/Hyper-H8/ST-LIB/Inc/HALAL/Services" -I"C:/Users/aleja/Proyectos/Hyper-H8/ST-LIB/Inc/ST-LIB_LOW" -I"C:/Users/aleja/Proyectos/Hyper-H8/ST-LIB/Inc/ST-LIB_HIGH" -I../Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-LTC681x

clean-Src-2f-LTC681x:
	-$(RM) ./Src/LTC681x/LTC681x.d ./Src/LTC681x/LTC681x.o ./Src/LTC681x/LTC681x.su

.PHONY: clean-Src-2f-LTC681x

