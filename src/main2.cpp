#include <iostream>
#include <cstdlib>

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
}

/* -------------------------------------------------- */
/* Globals                                            */
/* -------------------------------------------------- */

static QueueHandle_t gQueue = nullptr;
static SemaphoreHandle_t gSemaphore = nullptr;

/* -------------------------------------------------- */
/* Tasks                                              */
/* -------------------------------------------------- */

void vProducerTask(void* pvParameters)
{
    int value = 0;

    for (;;)
    {
        if (xQueueSend(gQueue, &value, portMAX_DELAY) == pdPASS)
        {
            std::cout << "[Producer] Sent: " << value << std::endl << std::flush;
            value++;
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vConsumerTask(void* pvParameters)
{
    int received = 0;

    for (;;)
    {
        if (xQueueReceive(gQueue, &received, portMAX_DELAY) == pdPASS)
        {
            std::cout << "[Consumer] Received: " << received << std::endl << std::flush;
        }
    }
}

void vGiveSemaphoreTask(void* pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(2000));

    std::cout << "[GiveSemaphoreTask] Gave semaphore" << std::endl << std::flush;
    xSemaphoreGive(gSemaphore);

    vTaskDelete(nullptr);
}

void vTakeSemaphoreTask(void* pvParameters)
{
    std::cout << "[TakeSemaphoreTask] Waiting for semaphore..." << std::endl << std::flush;

    if (xSemaphoreTake(gSemaphore, portMAX_DELAY) == pdTRUE)
    {
        std::cout << "[TakeSemaphoreTask] Semaphore taken!" << std::endl << std::flush;
    }

    vTaskDelete(nullptr);
}

void vSelfDeleteTask(void* pvParameters)
{
    for (int i = 0; i < 3; i++)
    {
        std::cout << "[SelfDeleteTask] Tick " << i << std::endl << std::flush;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    std::cout << "[SelfDeleteTask] Deleting itself" << std::endl << std::flush;
    vTaskDelete(nullptr);
}

void vMonitorTask(void* pvParameters)
{
    for (;;)
    {
        std::cout << "[Monitor] Scheduler running..." << std::endl << std::flush;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* Win32 ONLY: stop scheduler after demo time */
void vControllerTask(void* pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(10000));

    std::cout << "[Controller] Ending scheduler" << std::endl << std::flush;

    /* Win32 FreeRTOS ONLY */
    vTaskEndScheduler();
}

/* -------------------------------------------------- */
/* main                                               */
/* -------------------------------------------------- */

int main()
{
    std::cout << "=== FreeRTOS Win32 Simulator ===" << std::endl << std::flush;

    gQueue = xQueueCreate(5, sizeof(int));
    gSemaphore = xSemaphoreCreateBinary();

    if (!gQueue || !gSemaphore)
    {
        std::cerr << "Failed to create queue or semaphore" << std::endl;
        return -1;
    }

    xTaskCreate(vProducerTask, "Producer", 1024, nullptr, 2, nullptr);
    xTaskCreate(vConsumerTask, "Consumer", 1024, nullptr, 2, nullptr);
    xTaskCreate(vGiveSemaphoreTask, "GiveSem", 1024, nullptr, 1, nullptr);
    xTaskCreate(vTakeSemaphoreTask, "TakeSem", 1024, nullptr, 1, nullptr);
    xTaskCreate(vSelfDeleteTask, "SelfDelete", 1024, nullptr, 1, nullptr);
    xTaskCreate(vMonitorTask, "Monitor", 1024, nullptr, 0, nullptr);
    xTaskCreate(vControllerTask, "Controller", 1024, nullptr, 3, nullptr);

    vTaskStartScheduler();

    /* Returned after vTaskEndScheduler() */
    std::cout << "=== FreeRTOS Scheduler stopped ===" << std::endl << std::flush;
    return 0;
}
