#pragma once

#ifdef USE_HAL_DRIVER
extern "C" {
#include "cmsis_os2.h"
// #include "cmsis_os.h"
void HAL_Delay(uint32_t Delay);
}
#endif





class Delay {
public:
    static void osDelayMs(std::uint32_t timeoutMs) noexcept {
        osDelay(timeoutMs * kUsPerMs);
    }

    static void halDelayMs(std::uint32_t timeoutMs) noexcept {
        HAL_Delay(timeoutMs);
    }

    static void delayUs(std::uint32_t timeoutUs) noexcept {
        busyWaitUs(timeoutUs);
    }

private:
    static constexpr std::uint32_t kUsPerMs = 1000U;

    static void busyWaitUs(std::uint32_t timeoutUs) noexcept {
    }
};
