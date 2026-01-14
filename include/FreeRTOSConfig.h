#pragma once

/* Kernel configuration macros */
#define configUSE_PREEMPTION            1
#define configUSE_TIME_SLICING          1
#define configCPU_CLOCK_HZ              ( ( unsigned long ) 100000000 )
#define configTICK_RATE_HZ              ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES            7
#define configMINIMAL_STACK_SIZE        128
#define configTOTAL_HEAP_SIZE           ( 1024 * 1024 )
#define configMAX_TASK_NAME_LEN         16
#define configUSE_MUTEXES               1
#define configUSE_COUNTING_SEMAPHORES   1
#define configUSE_RECURSIVE_MUTEXES     1
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configSUPPORT_STATIC_ALLOCATION  0
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0
#define INCLUDE_vTaskDelete             1
#define INCLUDE_vTaskDelay              1
#define INCLUDE_vTaskSuspend            1
#define INCLUDE_xTaskGetSchedulerState  1
#define configUSE_16_BIT_TICKS          0
#define configASSERT(x) if((x)==0) { __debugbreak(); }
