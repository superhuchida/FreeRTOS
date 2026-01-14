#include <iostream>

/* FreeRTOS includes must be C-compatible */
extern "C" {
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
}

/* ============================================================
   C++ RAII FreeRTOS Mutex
   ============================================================ */

class FreeRTOSMutex
{
public:
    FreeRTOSMutex()
    {
        m_mutex = xSemaphoreCreateMutex();
        configASSERT(m_mutex != nullptr);
    }

    ~FreeRTOSMutex()
    {
        if (m_mutex)
        {
            vSemaphoreDelete(m_mutex);
        }
    }

    /* Non-copyable */
    FreeRTOSMutex(const FreeRTOSMutex&) = delete;
    FreeRTOSMutex& operator=(const FreeRTOSMutex&) = delete;

    void lock(TickType_t timeout = portMAX_DELAY)
    {
        BaseType_t ok = xSemaphoreTake(m_mutex, timeout);
        configASSERT(ok == pdTRUE);
    }

    void unlock()
    {
        BaseType_t ok = xSemaphoreGive(m_mutex);
        configASSERT(ok == pdTRUE);
    }

private:
    SemaphoreHandle_t m_mutex;
};

class LockGuard
{
public:
    explicit LockGuard(FreeRTOSMutex& mutex,
                       TickType_t timeout = portMAX_DELAY)
        : m_mutex(mutex)
    {
        m_mutex.lock(timeout);
    }

    ~LockGuard()
    {
        m_mutex.unlock();
    }

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    FreeRTOSMutex& m_mutex;
};

/* ============================================================
   Globals
   ============================================================ */

static FreeRTOSMutex gSharedMutex;

/* ============================================================
   Task Forward Declarations
   ============================================================ */

void vLowPriorityTask(void*);
void vMediumPriorityTask(void*);
void vHighPriorityTask(void*);
void vControllerTask(void*);

/* ============================================================
   Tasks
   ============================================================ */

/* LOW priority task: takes mutex and holds it */
void vLowPriorityTask(void*)
{
    for (;;)
    {
        {
            LockGuard lock(gSharedMutex);
            std::cout << "[LOW ] Holding mutex (long work)"
                      << std::endl << std::flush;

            vTaskDelay(pdMS_TO_TICKS(3000));
        } // mutex released automatically

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* MEDIUM priority task: CPU hog (no mutex) */
void vMediumPriorityTask(void*)
{
    for (;;)
    {
        std::cout << "[MED ] Running" << std::endl << std::flush;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* HIGH priority task: needs mutex */
void vHighPriorityTask(void*)
{
    /* Let LOW grab mutex first */
    vTaskDelay(pdMS_TO_TICKS(1000));

    for (;;)
    {
        std::cout << "[HIGH] Waiting for mutex"
                  << std::endl << std::flush;

        {
            LockGuard lock(gSharedMutex);
            std::cout << "[HIGH] Acquired mutex!"
                      << std::endl << std::flush;

            vTaskDelay(pdMS_TO_TICKS(500));
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/* Win32-only controller: stop scheduler */
void vControllerTask(void*)
{
    vTaskDelay(pdMS_TO_TICKS(12000));

    std::cout << "[CTRL] Ending scheduler"
              << std::endl << std::flush;

    /* Win32 / POSIX only */
    vTaskEndScheduler();
}

/* ============================================================
   main
   ============================================================ */

int main()
{
    std::cout << "=== FreeRTOS C++ RAII Mutex Demo ==="
              << std::endl << std::flush;

    xTaskCreate(vLowPriorityTask,
                "Low",
                1024,
                nullptr,
                1,
                nullptr);

    xTaskCreate(vMediumPriorityTask,
                "Med",
                1024,
                nullptr,
                2,
                nullptr);

    xTaskCreate(vHighPriorityTask,
                "High",
                1024,
                nullptr,
                3,
                nullptr);

    xTaskCreate(vControllerTask,
                "Ctrl",
                1024,
                nullptr,
                4,
                nullptr);

    vTaskStartScheduler();

    std::cout << "=== Scheduler stopped ==="
              << std::endl << std::flush;

    return 0;
}
