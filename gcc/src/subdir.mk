# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/func.cpp \
../src/socket.cpp \
../src/socket_handler.cpp 

OBJS += \
./src/func.o \
./src/socket.o \
./src/socket_handler.o 

CPP_DEPS += \
./src/func.d \
./src/socket.d \
./src/socket_handler.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -I"../../core/src" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


