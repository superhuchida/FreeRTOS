cmake_minimum_required(VERSION 3.15)
project(freertos_app C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add FreeRTOS kernel sources
file(GLOB FREERTOS_KERNEL_C
    FreeRTOS-Kernel/*.c
    FreeRTOS-Kernel/portable/MSVC-MingW/*.c
    FreeRTOS-Kernel/portable/MemMang/heap_4.c
)

# Add your application sources
set(APP_SOURCES
    src/main.cpp
    src/hooks.cpp
)

# Create executable
add_executable(freertos_app
    ${APP_SOURCES}
    ${FREERTOS_KERNEL_C}
)

# Include directories
target_include_directories(freertos_app PRIVATE
    FreeRTOS-Kernel/include
    FreeRTOS-Kernel/portable/MSVC-MingW
    include
)

# Link Windows multimedia library for timers
target_link_libraries(freertos_app PRIVATE winmm)
