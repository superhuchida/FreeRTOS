# FreeRTOS demo under Win 11 

## Project structure
```bash
FreeRTOS-Win32/
├─ CMakeLists.txt
├─ include/
│  └─ FreeRTOSConfig.h
├─ src/
│  ├─ main.cpp
│  ├─ hooks.cpp
│  └─ FreeRTOSMutex.hpp
├─ FreeRTOS-Kernel/
│  ├─ include/
│  │  └─ *.h (FreeRTOS kernel headers)
│  ├─ portable/
│  │  └─ MSVC-MingW/
│  │     └─ port.c
│  └─ *.c (kernel source files)
```
## Build (using a CMakeLists.txt file)
mkdir build && cd build

cmake .. -G Ninja

cmake --build .


## Run
./freertos_app.exe
