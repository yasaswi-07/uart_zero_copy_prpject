################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Src/UART.c \
../Drivers/Src/stm32401xx_gpiodriver.c \
../Drivers/Src/stm32f401xxxrcc_driver.c 

OBJS += \
./Drivers/Src/UART.o \
./Drivers/Src/stm32401xx_gpiodriver.o \
./Drivers/Src/stm32f401xxxrcc_driver.o 

C_DEPS += \
./Drivers/Src/UART.d \
./Drivers/Src/stm32401xx_gpiodriver.d \
./Drivers/Src/stm32f401xxxrcc_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Src/%.o Drivers/Src/%.su Drivers/Src/%.cyclo: ../Drivers/Src/%.c Drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F401RBTx -DSTM32F4 -c -I"D:/MCU_1/workspaceMCU/UART_zero_copy/Drivers/Inc" -I"D:/MCU_1/workspaceMCU/UART_zero_copy/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Src

clean-Drivers-2f-Src:
	-$(RM) ./Drivers/Src/UART.cyclo ./Drivers/Src/UART.d ./Drivers/Src/UART.o ./Drivers/Src/UART.su ./Drivers/Src/stm32401xx_gpiodriver.cyclo ./Drivers/Src/stm32401xx_gpiodriver.d ./Drivers/Src/stm32401xx_gpiodriver.o ./Drivers/Src/stm32401xx_gpiodriver.su ./Drivers/Src/stm32f401xxxrcc_driver.cyclo ./Drivers/Src/stm32f401xxxrcc_driver.d ./Drivers/Src/stm32f401xxxrcc_driver.o ./Drivers/Src/stm32f401xxxrcc_driver.su

.PHONY: clean-Drivers-2f-Src

