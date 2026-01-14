#include <iostream>

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
}

/*
    * This example demonstrates a deadlock situation in FreeRTOS
    * using two tasks and two mutexes. Task1 locks MutexA then MutexB,
    * while Task2 locks MutexB then MutexA, leading to a potential deadlock.
*/  


static SemaphoreHandle_t gMutexA;
static SemaphoreHandle_t gMutexB;

/*
void vTask1(void*)
{
    for (;;)
    {
        std::cout << "[Task1] Locking A" << std::endl << std::flush;
        xSemaphoreTake(gMutexA, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(500));

        std::cout << "[Task1] Locking B" << std::endl << std::flush;
        xSemaphoreTake(gMutexB, portMAX_DELAY);

        std::cout << "[Task1] Got A+B" << std::endl << std::flush;

        xSemaphoreGive(gMutexB);
        xSemaphoreGive(gMutexA);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTask2(void*)
{
    for (;;)
    {
        std::cout << "[Task2] Locking B" << std::endl << std::flush;
        xSemaphoreTake(gMutexB, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(500));

        std::cout << "[Task2] Locking A" << std::endl << std::flush;
        xSemaphoreTake(gMutexA, portMAX_DELAY);

        std::cout << "[Task2] Got B+A" << std::endl << std::flush;

        xSemaphoreGive(gMutexA);
        xSemaphoreGive(gMutexB);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
*/

/* Fixed version (Lock Ordering) */
void vTask1(void*)
{
    for (;;)
    {
        xSemaphoreTake(gMutexA, portMAX_DELAY);
        xSemaphoreTake(gMutexB, portMAX_DELAY);

        std::cout << "[Task1] Got A+B" << std::endl << std::flush;

        xSemaphoreGive(gMutexB);
        xSemaphoreGive(gMutexA);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTask2(void*)
{
    for (;;)
    {
        xSemaphoreTake(gMutexA, portMAX_DELAY);
        xSemaphoreTake(gMutexB, portMAX_DELAY);

        std::cout << "[Task2] Got A+B" << std::endl << std::flush;

        xSemaphoreGive(gMutexB);
        xSemaphoreGive(gMutexA);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vControllerTask(void*)
{
    vTaskDelay(pdMS_TO_TICKS(8000));
    std::cout << "[CTRL] System deadlocked. Stopping scheduler." << std::endl;
    vTaskEndScheduler();
}

int main()
{
    std::cout << "=== DEADLOCK DEMO ===" << std::endl << std::flush;

    gMutexA = xSemaphoreCreateMutex();
    gMutexB = xSemaphoreCreateMutex();

    xTaskCreate(vTask1, "Task1", 1024, nullptr, 2, nullptr);
    xTaskCreate(vTask2, "Task2", 1024, nullptr, 2, nullptr);
    xTaskCreate(vControllerTask, "Ctrl", 1024, nullptr, 3, nullptr);

    vTaskStartScheduler();
    return 0;
}
