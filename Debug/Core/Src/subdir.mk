################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/CAN_bus.cpp \
../Core/Src/CmdLine.cpp \
../Core/Src/ForeverLoop.cpp \
../Core/Src/LevelCrossing.cpp \
../Core/Src/LinesideSignal.cpp \
../Core/Src/MechaniclaLock.cpp \
../Core/Src/MovingGate.cpp \
../Core/Src/Project.cpp \
../Core/Src/PulseAlarm.cpp \
../Core/Src/PushButton.cpp \
../Core/Src/Serial.cpp \
../Core/Src/Shift_Reg_SPI.cpp \
../Core/Src/SignalsMain.cpp \
../Core/Src/SingleInput.cpp \
../Core/Src/SingleOutput.cpp \
../Core/Src/SystemSetup.cpp \
../Core/Src/TurnoutPoints.cpp \
../Core/Src/Utils.cpp \
../Core/Src/microsecs.cpp \
../Core/Src/rtc.cpp \
../Core/Src/settings.cpp 

C_SRCS += \
../Core/Src/main.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l4xx.c 

C_DEPS += \
./Core/Src/main.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l4xx.d 

OBJS += \
./Core/Src/CAN_bus.o \
./Core/Src/CmdLine.o \
./Core/Src/ForeverLoop.o \
./Core/Src/LevelCrossing.o \
./Core/Src/LinesideSignal.o \
./Core/Src/MechaniclaLock.o \
./Core/Src/MovingGate.o \
./Core/Src/Project.o \
./Core/Src/PulseAlarm.o \
./Core/Src/PushButton.o \
./Core/Src/Serial.o \
./Core/Src/Shift_Reg_SPI.o \
./Core/Src/SignalsMain.o \
./Core/Src/SingleInput.o \
./Core/Src/SingleOutput.o \
./Core/Src/SystemSetup.o \
./Core/Src/TurnoutPoints.o \
./Core/Src/Utils.o \
./Core/Src/main.o \
./Core/Src/microsecs.o \
./Core/Src/rtc.o \
./Core/Src/settings.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l4xx.o 

CPP_DEPS += \
./Core/Src/CAN_bus.d \
./Core/Src/CmdLine.d \
./Core/Src/ForeverLoop.d \
./Core/Src/LevelCrossing.d \
./Core/Src/LinesideSignal.d \
./Core/Src/MechaniclaLock.d \
./Core/Src/MovingGate.d \
./Core/Src/Project.d \
./Core/Src/PulseAlarm.d \
./Core/Src/PushButton.d \
./Core/Src/Serial.d \
./Core/Src/Shift_Reg_SPI.d \
./Core/Src/SignalsMain.d \
./Core/Src/SingleInput.d \
./Core/Src/SingleOutput.d \
./Core/Src/SystemSetup.d \
./Core/Src/TurnoutPoints.d \
./Core/Src/Utils.d \
./Core/Src/microsecs.d \
./Core/Src/rtc.d \
./Core/Src/settings.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.cpp Core/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/CAN_bus.cyclo ./Core/Src/CAN_bus.d ./Core/Src/CAN_bus.o ./Core/Src/CAN_bus.su ./Core/Src/CmdLine.cyclo ./Core/Src/CmdLine.d ./Core/Src/CmdLine.o ./Core/Src/CmdLine.su ./Core/Src/ForeverLoop.cyclo ./Core/Src/ForeverLoop.d ./Core/Src/ForeverLoop.o ./Core/Src/ForeverLoop.su ./Core/Src/LevelCrossing.cyclo ./Core/Src/LevelCrossing.d ./Core/Src/LevelCrossing.o ./Core/Src/LevelCrossing.su ./Core/Src/LinesideSignal.cyclo ./Core/Src/LinesideSignal.d ./Core/Src/LinesideSignal.o ./Core/Src/LinesideSignal.su ./Core/Src/MechaniclaLock.cyclo ./Core/Src/MechaniclaLock.d ./Core/Src/MechaniclaLock.o ./Core/Src/MechaniclaLock.su ./Core/Src/MovingGate.cyclo ./Core/Src/MovingGate.d ./Core/Src/MovingGate.o ./Core/Src/MovingGate.su ./Core/Src/Project.cyclo ./Core/Src/Project.d ./Core/Src/Project.o ./Core/Src/Project.su ./Core/Src/PulseAlarm.cyclo ./Core/Src/PulseAlarm.d ./Core/Src/PulseAlarm.o ./Core/Src/PulseAlarm.su ./Core/Src/PushButton.cyclo ./Core/Src/PushButton.d ./Core/Src/PushButton.o ./Core/Src/PushButton.su ./Core/Src/Serial.cyclo ./Core/Src/Serial.d ./Core/Src/Serial.o ./Core/Src/Serial.su ./Core/Src/Shift_Reg_SPI.cyclo ./Core/Src/Shift_Reg_SPI.d ./Core/Src/Shift_Reg_SPI.o ./Core/Src/Shift_Reg_SPI.su ./Core/Src/SignalsMain.cyclo ./Core/Src/SignalsMain.d ./Core/Src/SignalsMain.o ./Core/Src/SignalsMain.su ./Core/Src/SingleInput.cyclo ./Core/Src/SingleInput.d ./Core/Src/SingleInput.o ./Core/Src/SingleInput.su ./Core/Src/SingleOutput.cyclo ./Core/Src/SingleOutput.d ./Core/Src/SingleOutput.o ./Core/Src/SingleOutput.su ./Core/Src/SystemSetup.cyclo ./Core/Src/SystemSetup.d ./Core/Src/SystemSetup.o ./Core/Src/SystemSetup.su ./Core/Src/TurnoutPoints.cyclo ./Core/Src/TurnoutPoints.d ./Core/Src/TurnoutPoints.o ./Core/Src/TurnoutPoints.su ./Core/Src/Utils.cyclo ./Core/Src/Utils.d ./Core/Src/Utils.o ./Core/Src/Utils.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/microsecs.cyclo ./Core/Src/microsecs.d ./Core/Src/microsecs.o ./Core/Src/microsecs.su ./Core/Src/rtc.cyclo ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/rtc.su ./Core/Src/settings.cyclo ./Core/Src/settings.d ./Core/Src/settings.o ./Core/Src/settings.su ./Core/Src/stm32l4xx_hal_msp.cyclo ./Core/Src/stm32l4xx_hal_msp.d ./Core/Src/stm32l4xx_hal_msp.o ./Core/Src/stm32l4xx_hal_msp.su ./Core/Src/stm32l4xx_it.cyclo ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/stm32l4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l4xx.cyclo ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/system_stm32l4xx.su

.PHONY: clean-Core-2f-Src

