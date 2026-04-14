#pragma once

#include <cstdint>
#include "Timestamp.hpp"

extern "C" {
#if __has_include("cmsis_os2.h")
#include "cmsis_os2.h"
#else
void osDelay(uint32_t timeoutMs);
#endif
}

namespace EP::Bsp::Stm32 {
class Delay {
public:
    static void osDelayMs(std::uint32_t timeoutMs) noexcept {
        osDelay(timeoutMs);
    }

    static void halDelayMs(std::uint32_t timeoutMs) noexcept {
        delayUs(timeoutMs * kUsPerMs);
    }

    static void delayUs(std::uint32_t timeoutUs) noexcept {
        busyWaitUs(timeoutUs);
    }

private:
    static constexpr std::uint32_t kUsPerMs = 1000U;

    static void busyWaitUs(std::uint32_t timeoutUs) noexcept {
        const auto start = SysTickCLK::Timestamp::now();
        while ((SysTickCLK::Timestamp::now() - start).totalMicroseconds() < timeoutUs) {
        }
    }
};
}
