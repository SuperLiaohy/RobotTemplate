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
        HAL_Delay(timeoutMs);
    }

    static void spinMs(std::uint32_t timeoutMs) noexcept {
        spinUs(timeoutMs * kUsPerMs);
    }

    static void spinUs(std::uint32_t timeoutUs) noexcept {
        busyWaitUs_2(timeoutUs);
    }

private:
    static constexpr std::uint32_t kUsPerMs = 1000U;

    [[gnu::always_inline]]
    static void busyWaitUs_1(std::uint32_t timeoutUs) noexcept {
        const auto start = SysTickCLK::Timestamp::now();
        while ((SysTickCLK::Timestamp::now() - start).totalMicroseconds() < timeoutUs) {
        }
    }

    [[gnu::always_inline]]
    static void busyWaitUs_2(std::uint32_t timeoutUs) noexcept {
        const std::uint32_t targetMs = timeoutUs / 1000U;
        const std::uint32_t targetSubTick =
            (timeoutUs % 1000U) * SysTickCLK::Timestamp::subTickPeriod() / 1000U;
        const auto start = SysTickCLK::Timestamp::now();
        while (true) {
            const auto diff = SysTickCLK::Timestamp::now() - start;
            if (diff.milliseconds() > targetMs) {
                break;
            }
            if (diff.milliseconds() == targetMs && diff.subTick() >= targetSubTick) {
                break;
            }
        }
    }

};
}
