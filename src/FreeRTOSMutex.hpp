#pragma once
#include "FreeRTOS.h"
#include "semphr.h"

class FreeRTOSMutex {
    SemaphoreHandle_t m_mutex;

public:
    FreeRTOSMutex()
    {
        m_mutex = xSemaphoreCreateMutex();
    }

    ~FreeRTOSMutex()
    {
        if (m_mutex)
            vSemaphoreDelete(m_mutex);
    }

    void lock(TickType_t timeout = portMAX_DELAY)
    {
        xSemaphoreTake(m_mutex, timeout);
    }

    void unlock()
    {
        xSemaphoreGive(m_mutex);
    }

    SemaphoreHandle_t get() { return m_mutex; }
};
