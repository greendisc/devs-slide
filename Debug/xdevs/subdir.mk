################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../xdevs/DevsJobsOffline.cpp \
../xdevs/DevsSlide.cpp \
../xdevs/DevsWeather.cpp \
../xdevs/main_xdevs.cpp 

OBJS += \
./xdevs/DevsJobsOffline.o \
./xdevs/DevsSlide.o \
./xdevs/DevsWeather.o \
./xdevs/main_xdevs.o 

CPP_DEPS += \
./xdevs/DevsJobsOffline.d \
./xdevs/DevsSlide.d \
./xdevs/DevsWeather.d \
./xdevs/main_xdevs.d 


# Each subdirectory must supply rules for building sources it contributes
xdevs/%.o: ../xdevs/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


