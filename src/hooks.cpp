#include "FreeRTOS.h"
#include "task.h"
#include <cstdio>
#include <cstdlib>

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
    printf("[HOOK] Stack overflow in task: %s\n", pcTaskName);
    std::abort();
}
