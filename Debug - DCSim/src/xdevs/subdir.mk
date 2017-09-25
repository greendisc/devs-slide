################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/xdevs/DevsJobsOffline.cpp \
../src/xdevs/DevsSlide.cpp \
../src/xdevs/DevsWeather.cpp \
../src/xdevs/main_xdevs.cpp 

OBJS += \
./src/xdevs/DevsJobsOffline.o \
./src/xdevs/DevsSlide.o \
./src/xdevs/DevsWeather.o \
./src/xdevs/main_xdevs.o 

CPP_DEPS += \
./src/xdevs/DevsJobsOffline.d \
./src/xdevs/DevsSlide.d \
./src/xdevs/DevsWeather.d \
./src/xdevs/main_xdevs.d 


# Each subdirectory must supply rules for building sources it contributes
src/xdevs/%.o: ../src/xdevs/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


