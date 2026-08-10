################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f4xx.c 

OBJS += \
./src/main.o \
./src/syscalls.o \
./src/system_stm32f4xx.o 

C_DEPS += \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F407VETx -DDEBUG -DSTM32F40XX -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -I"C:/Users/alejo/OneDrive/Desktop/UTN/Tecnicas Digitales II/Laboratorios/TP4_2026_Hernandez/StdPeriph_Driver/inc" -I"C:/Users/alejo/OneDrive/Desktop/UTN/Tecnicas Digitales II/Laboratorios/TP4_2026_Hernandez/inc" -I"C:/Users/alejo/OneDrive/Desktop/UTN/Tecnicas Digitales II/Laboratorios/TP4_2026_Hernandez/CMSIS/device" -I"C:/Users/alejo/OneDrive/Desktop/UTN/Tecnicas Digitales II/Laboratorios/TP4_2026_Hernandez/CMSIS/core" -I"C:/Users/alejo/OneDrive/Desktop/UTN/Tecnicas Digitales II/Laboratorios/TP4_2026_Hernandez/GLOBAL" -I"C:/Users/alejo/OneDrive/Desktop/UTN/Tecnicas Digitales II/Laboratorios/TP4_2026_Hernandez/LCD" -I"C:/Users/alejo/OneDrive/Desktop/UTN/Tecnicas Digitales II/Laboratorios/TP4_2026_Hernandez/Menu" -I"C:/Users/alejo/OneDrive/Desktop/UTN/Tecnicas Digitales II/Laboratorios/TP4_2026_Hernandez/Teclado" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


