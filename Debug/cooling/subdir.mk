################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cooling/ashrae-pump.cpp \
../cooling/chiller.cpp \
../cooling/custom-chiller.cpp \
../cooling/plantB-chiller.cpp \
../cooling/pump.cpp 

OBJS += \
./cooling/ashrae-pump.o \
./cooling/chiller.o \
./cooling/custom-chiller.o \
./cooling/plantB-chiller.o \
./cooling/pump.o 

CPP_DEPS += \
./cooling/ashrae-pump.d \
./cooling/chiller.d \
./cooling/custom-chiller.d \
./cooling/plantB-chiller.d \
./cooling/pump.d 


# Each subdirectory must supply rules for building sources it contributes
cooling/%.o: ../cooling/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


