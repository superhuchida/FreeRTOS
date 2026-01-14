#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOSMutex.hpp"
#include <cstdio>

// Queue demo
QueueHandle_t q;
SemaphoreHandle_t sem;
FreeRTOSMutex mutex;

// ---------------- Producer / Consumer ----------------
void vProducerTask(void* p) {
    int counter = 0;
    while (1) {
        xQueueSend(q, &counter, portMAX_DELAY);
        printf("[Producer] Sent: %d\n", counter++);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void vConsumerTask(void* p) {
    int val;
    while (1) {
        xQueueReceive(q, &val, portMAX_DELAY);
        printf("[Consumer] Received: %d\n", val);
    }
}

// ---------------- Semaphore demo ----------------
void vGiveSemaphoreTask(void*) {
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        xSemaphoreGive(sem);
        printf("[GiveSemaphoreTask] Gave semaphore\n");
    }
}

void vTakeSemaphoreTask(void*) {
    while (1) {
        if (xSemaphoreTake(sem, portMAX_DELAY))
            printf("[TakeSemaphoreTask] Took semaphore\n");
    }
}

// ---------------- Self-delete demo ----------------
void vSelfDeleteTask(void*) {
    int tick = 0;
    while (tick < 5) {
        printf("[SelfDeleteTask] Tick %d\n", tick++);
        vTaskDelay(700 / portTICK_PERIOD_MS);
    }
    printf("[SelfDeleteTask] Deleting self\n");
    vTaskDelete(nullptr);
}

// ---------------- Mutex priority demo ----------------
void vMutexTaskLow(void*) {
    while (1) {
        mutex.lock();
        printf("[LowPriority] Got mutex\n");
        vTaskDelay(1500 / portTICK_PERIOD_MS);
        mutex.unlock();
        printf("[LowPriority] Released mutex\n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void vMutexTaskHigh(void*) {
    while (1) {
        mutex.lock();
        printf("[HighPriority] Got mutex\n");
        mutex.unlock();
        printf("[HighPriority] Released mutex\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// ---------------- main ----------------
int main() {
    q = xQueueCreate(5, sizeof(int));
    sem = xSemaphoreCreateBinary();

    // Producer / Consumer
    xTaskCreate(vProducerTask, "Producer", 1024, nullptr, 2, nullptr);
    xTaskCreate(vConsumerTask, "Consumer", 1024, nullptr, 2, nullptr);

    // Semaphore tasks
    xTaskCreate(vGiveSemaphoreTask, "GiveSem", 1024, nullptr, 3, nullptr);
    xTaskCreate(vTakeSemaphoreTask, "TakeSem", 1024, nullptr, 3, nullptr);

    // Self-delete demo
    xTaskCreate(vSelfDeleteTask, "SelfDelete", 1024, nullptr, 1, nullptr);

    // Mutex + priority demo
    xTaskCreate(vMutexTaskLow, "LowPrio", 1024, nullptr, 1, nullptr);
    xTaskCreate(vMutexTaskHigh, "HighPrio", 1024, nullptr, 3, nullptr);

    vTaskStartScheduler();

    return 0;
}
