#pragma once

extern "C" {
#include "FreeRTOS.h"
#include "semphr.h"
}

/* ---------------------------------------------
   FreeRTOS Mutex (RAII)
---------------------------------------------- */

class FreeRTOSMutex
{
public:
    FreeRTOSMutex()
    {
        m_mutex = xSemaphoreCreateMutex();
        configASSERT(m_mutex != nullptr);
    }

    /* Non-copyable */
    FreeRTOSMutex(const FreeRTOSMutex&) = delete;
    FreeRTOSMutex& operator=(const FreeRTOSMutex&) = delete;

    /* Movable (optional) */
    FreeRTOSMutex(FreeRTOSMutex&& other) noexcept
        : m_mutex(other.m_mutex)
    {
        other.m_mutex = nullptr;
    }

    ~FreeRTOSMutex()
    {
        if (m_mutex)
        {
            vSemaphoreDelete(m_mutex);
        }
    }

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

    SemaphoreHandle_t native_handle() const
    {
        return m_mutex;
    }

private:
    SemaphoreHandle_t m_mutex;
};

/* ---------------------------------------------
   Lock Guard (RAII)
---------------------------------------------- */

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

    /* Non-copyable */
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    FreeRTOSMutex& m_mutex;
};
