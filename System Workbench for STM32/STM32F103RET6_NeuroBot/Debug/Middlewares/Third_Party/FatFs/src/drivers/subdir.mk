################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FatFs/src/drivers/sd_diskio.c 

OBJS += \
./Middlewares/Third_Party/FatFs/src/drivers/sd_diskio.o 

C_DEPS += \
./Middlewares/Third_Party/FatFs/src/drivers/sd_diskio.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FatFs/src/drivers/%.o: ../Middlewares/Third_Party/FatFs/src/drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F103xE -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/STM32F1xx_HAL_Driver/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/Third_Party/FatFs/src/drivers" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/Third_Party/FatFs/src" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/CMSIS/Include"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -fdata-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


