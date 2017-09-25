################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cooling/ashrae-pump.cpp \
../src/cooling/chiller.cpp \
../src/cooling/custom-chiller.cpp \
../src/cooling/plantB-chiller.cpp \
../src/cooling/pump.cpp 

OBJS += \
./src/cooling/ashrae-pump.o \
./src/cooling/chiller.o \
./src/cooling/custom-chiller.o \
./src/cooling/plantB-chiller.o \
./src/cooling/pump.o 

CPP_DEPS += \
./src/cooling/ashrae-pump.d \
./src/cooling/chiller.d \
./src/cooling/custom-chiller.d \
./src/cooling/plantB-chiller.d \
./src/cooling/pump.d 


# Each subdirectory must supply rules for building sources it contributes
src/cooling/%.o: ../src/cooling/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/jlrisco/TrabajoExtra/GIT/jlrisco@github.com/devs-slide/lib/xdevs" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


