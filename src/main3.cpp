#include <iostream>

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
}
/*
🧵 FreeRTOS Mutex & Priority Inheritance Demo (Win32)
What this demo proves

We create 3 tasks:

Task	Priority	Behavior
🟥 LowPriorityTask	1	Takes mutex and holds it
🟨 MediumPriorityTask	2	CPU hog (no mutex)
🟩 HighPriorityTask	3	Needs mutex
Expected behavior

✔ When High blocks on the mutex held by Low,
✔ FreeRTOS boosts Low’s priority to match High,
✔ Medium cannot starve Low,
✔ Mutex is released quickly → High runs.

Without priority inheritance → deadlock-like starvation.
*/

/* -------------------------------------------------- */
/* Globals                                            */
/* -------------------------------------------------- */

static SemaphoreHandle_t gMutex;

/* -------------------------------------------------- */
/* Tasks                                              */
/* -------------------------------------------------- */

void vLowPriorityTask(void* pvParameters)
{
    for (;;)
    {
        std::cout << "[LOW ] Trying to take mutex" << std::endl << std::flush;
        xSemaphoreTake(gMutex, portMAX_DELAY);

        std::cout << "[LOW ] Mutex taken, doing long work" << std::endl << std::flush;

        /* Simulate long critical section */
        vTaskDelay(pdMS_TO_TICKS(3000));

        std::cout << "[LOW ] Releasing mutex" << std::endl << std::flush;
        xSemaphoreGive(gMutex);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vMediumPriorityTask(void* pvParameters)
{
    for (;;)
    {
        std::cout << "[MED ] Running (CPU hog)" << std::endl << std::flush;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vHighPriorityTask(void* pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(1000)); // Let LOW take mutex first

    for (;;)
    {
        std::cout << "[HIGH] Trying to take mutex" << std::endl << std::flush;
        xSemaphoreTake(gMutex, portMAX_DELAY);

        std::cout << "[HIGH] Mutex acquired!" << std::endl << std::flush;

        vTaskDelay(pdMS_TO_TICKS(500));

        std::cout << "[HIGH] Releasing mutex" << std::endl << std::flush;
        xSemaphoreGive(gMutex);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/* Stop scheduler after demo */
void vControllerTask(void* pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(12000));
    std::cout << "[CTRL] Ending scheduler" << std::endl << std::flush;
    vTaskEndScheduler();
}

/* -------------------------------------------------- */
/* main                                               */
/* -------------------------------------------------- */

int main()
{
    std::cout << "=== FreeRTOS Mutex & Priority Inheritance Demo ==="
              << std::endl << std::flush;

    gMutex = xSemaphoreCreateMutex();
    /*
    Result:

    ❌ No priority inheritance
    ❌ Medium starves Low
    ❌ High waits forever

    This is why binary semaphores are NOT mutexes.
    
    */

    // gMutex = xSemaphoreCreateBinary();

    if (!gMutex)
    {
        std::cerr << "Failed to create mutex" << std::endl;
        return -1;
    }

    xTaskCreate(vLowPriorityTask, "Low", 1024, nullptr, 1, nullptr);
    xTaskCreate(vMediumPriorityTask, "Med", 1024, nullptr, 2, nullptr);
    xTaskCreate(vHighPriorityTask, "High", 1024, nullptr, 3, nullptr);
    xTaskCreate(vControllerTask, "Ctrl", 1024, nullptr, 4, nullptr);

    vTaskStartScheduler();

    std::cout << "=== Scheduler stopped ===" << std::endl << std::flush;
    return 0;
}
