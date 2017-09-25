################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../room/apc-irc.cpp \
../room/decathlete-server.cpp \
../room/dell-server.cpp \
../room/factory.cpp \
../room/irc.cpp \
../room/mghpcc-pod.cpp \
../room/rack-irc.cpp \
../room/rack.cpp \
../room/room.cpp \
../room/server.cpp \
../room/solana-server.cpp 

OBJS += \
./room/apc-irc.o \
./room/decathlete-server.o \
./room/dell-server.o \
./room/factory.o \
./room/irc.o \
./room/mghpcc-pod.o \
./room/rack-irc.o \
./room/rack.o \
./room/room.o \
./room/server.o \
./room/solana-server.o 

CPP_DEPS += \
./room/apc-irc.d \
./room/decathlete-server.d \
./room/dell-server.d \
./room/factory.d \
./room/irc.d \
./room/mghpcc-pod.d \
./room/rack-irc.d \
./room/rack.d \
./room/room.d \
./room/server.d \
./room/solana-server.d 


# Each subdirectory must supply rules for building sources it contributes
room/%.o: ../room/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


