#include <iostream>
#include "FreeRTOS.h"
#include "task.h"

void vTaskFunction(void* pvParameters)
{
    const char* name = static_cast<const char*>(pvParameters);
    for (;;)
    {
        std::cout << "Hello from " << name << std::endl;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main()
{
    xTaskCreate(vTaskFunction, "Task1", 1024, (void*)"Task1", 1, nullptr);
    xTaskCreate(vTaskFunction, "Task2", 1024, (void*)"Task2", 1, nullptr);

    vTaskStartScheduler(); // Start FreeRTOS

    return 0;
}
