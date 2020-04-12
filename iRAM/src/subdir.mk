################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cint_tc27x.c \
../src/hello.c \
../src/interrupts.c \
../src/system_tc27x.c \
../src/timer.c 

OBJS += \
./src/cint_tc27x.o \
./src/hello.o \
./src/interrupts.o \
./src/system_tc27x.o \
./src/timer.o 

C_DEPS += \
./src/cint_tc27x.d \
./src/hello.d \
./src/interrupts.d \
./src/system_tc27x.d \
./src/timer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TriCore C Compiler'
	"$(TRICORE_TOOLS)/bin/tricore-gcc" -c -I"../h" -fno-common -Os -g3 -W -Wall -Wextra -Wdiv-by-zero -Warray-bounds -Wcast-align -Wignored-qualifiers -Wformat -Wformat-security -pipe -DSHIELDBUDDY_TC275D -DUSE_IRQ -fshort-double -mcpu=tc27xx -mversion-info -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


