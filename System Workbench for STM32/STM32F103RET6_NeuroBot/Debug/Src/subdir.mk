################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/NeuroBot.c \
../Src/bsp_driver_sd.c \
../Src/fatfs.c \
../Src/main.c \
../Src/ssd1306.c \
../Src/ssd1306_fonts.c \
../Src/ssd1306_tests.c \
../Src/stm32f1xx_hal_msp.c \
../Src/stm32f1xx_it.c \
../Src/system_stm32f1xx.c \
../Src/tof.c \
../Src/usb_device.c \
../Src/usbd_conf.c \
../Src/usbd_desc.c \
../Src/usbd_storage_if.c 

CPP_SRCS += \
../Src/Neural_Network.cpp 

OBJS += \
./Src/Neural_Network.o \
./Src/NeuroBot.o \
./Src/bsp_driver_sd.o \
./Src/fatfs.o \
./Src/main.o \
./Src/ssd1306.o \
./Src/ssd1306_fonts.o \
./Src/ssd1306_tests.o \
./Src/stm32f1xx_hal_msp.o \
./Src/stm32f1xx_it.o \
./Src/system_stm32f1xx.o \
./Src/tof.o \
./Src/usb_device.o \
./Src/usbd_conf.o \
./Src/usbd_desc.o \
./Src/usbd_storage_if.o 

C_DEPS += \
./Src/NeuroBot.d \
./Src/bsp_driver_sd.d \
./Src/fatfs.d \
./Src/main.d \
./Src/ssd1306.d \
./Src/ssd1306_fonts.d \
./Src/ssd1306_tests.d \
./Src/stm32f1xx_hal_msp.d \
./Src/stm32f1xx_it.d \
./Src/system_stm32f1xx.d \
./Src/tof.d \
./Src/usb_device.d \
./Src/usbd_conf.d \
./Src/usbd_desc.d \
./Src/usbd_storage_if.d 

CPP_DEPS += \
./Src/Neural_Network.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F103xE -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/STM32F1xx_HAL_Driver/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/Third_Party/FatFs/src/drivers" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/Third_Party/FatFs/src" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F103xE -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/STM32F1xx_HAL_Driver/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/Third_Party/FatFs/src/drivers" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Middlewares/Third_Party/FatFs/src" -I"D:/Google Drive/System Workbench workspace/STM32F103RET6_NeuroBot/Drivers/CMSIS/Include"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -fdata-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


