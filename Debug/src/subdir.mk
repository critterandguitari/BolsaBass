################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CS4344.c \
../src/comb.c \
../src/cr_startup_stm32f4xx.c \
../src/line.c \
../src/main.c \
../src/midi.c \
../src/mode_drum.c \
../src/mode_drum_synth.c \
../src/mode_filter_envelope.c \
../src/mode_filter_man.c \
../src/mode_mono_glider.c \
../src/mode_nazareth.c \
../src/mode_simple_fm.c \
../src/mode_simple_sin.c \
../src/oscillator.c \
../src/pp6.c \
../src/sad.c \
../src/sadsr.c \
../src/system_stm32f4xx.c \
../src/timer.c \
../src/uart.c \
../src/vcf.c 

OBJS += \
./src/CS4344.o \
./src/comb.o \
./src/cr_startup_stm32f4xx.o \
./src/line.o \
./src/main.o \
./src/midi.o \
./src/mode_drum.o \
./src/mode_drum_synth.o \
./src/mode_filter_envelope.o \
./src/mode_filter_man.o \
./src/mode_mono_glider.o \
./src/mode_nazareth.o \
./src/mode_simple_fm.o \
./src/mode_simple_sin.o \
./src/oscillator.o \
./src/pp6.o \
./src/sad.o \
./src/sadsr.o \
./src/system_stm32f4xx.o \
./src/timer.o \
./src/uart.o \
./src/vcf.o 

C_DEPS += \
./src/CS4344.d \
./src/comb.d \
./src/cr_startup_stm32f4xx.d \
./src/line.d \
./src/main.d \
./src/midi.d \
./src/mode_drum.d \
./src/mode_drum_synth.d \
./src/mode_filter_envelope.d \
./src/mode_filter_man.d \
./src/mode_mono_glider.d \
./src/mode_nazareth.d \
./src/mode_simple_fm.d \
./src/mode_simple_sin.d \
./src/oscillator.d \
./src/pp6.d \
./src/sad.d \
./src/sadsr.d \
./src/system_stm32f4xx.d \
./src/timer.d \
./src/uart.d \
./src/vcf.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -DSTM32F4XX -DUSE_STDPERIPH_DRIVER -D__USE_CMSIS=CMSISv2p10_STM32F4xx -D__USE_CMSIS_DSPLIB=CMSISv2p10_DSPLIB_CM4 -DUSE_OLD_STYLE_DATA_BSS_INIT -I"/home/owen/workspace-redsuite/StdPeriphLib_v100_STM32F4xx/inc" -I"/home/owen/workspace-redsuite/CMSISv2p10_STM32F4xx/inc" -I"/home/owen/workspace-redsuite/CMSISv2p10_DSPLIB_CM4/inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


