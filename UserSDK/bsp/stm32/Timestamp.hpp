#pragma once

#include <cstdint>
#include "../../component/Format.h"

extern "C" {
#if __has_include("cmsis_os2.h")
#include "cmsis_os2.h"
inline auto _getTick() noexcept -> std::uint32_t {
    return osKernelGetTickCount();
}
#else
uint32_t HAL_GetTick(void);
inline auto _getTick() noexcept -> std::uint32_t {
    return HAL_GetTick();
}
#endif


}

namespace EP::Bsp::Stm32::SysTickCLK {

class Timestamp {
public:
    constexpr Timestamp() noexcept = default;

    constexpr Timestamp(std::uint32_t tickMs, std::uint32_t subTick) noexcept
        : tickMs_(tickMs), subTick_(subTick) {
    }

    [[nodiscard]] static Timestamp now() noexcept {
        std::uint32_t tickBefore = 0U;
        std::uint32_t tickAfter = 0U;
        std::uint32_t systickVal = 0U;
        do {
            tickBefore = _getTick();
            systickVal = SysTick->VAL;
            tickAfter = _getTick();
        } while (tickBefore != tickAfter);
        const std::uint32_t elapsedSubTick = subTickPeriod() - 1 - systickVal;
        return {tickAfter, elapsedSubTick};
    }

    [[nodiscard]] static std::uint32_t tickMs() noexcept {
        return _getTick();
    }

    [[nodiscard]] constexpr std::uint32_t milliseconds() const noexcept {
        return tickMs_;
    }

    [[nodiscard]] constexpr std::uint32_t subTick() const noexcept {
        return subTick_;
    }

    [[nodiscard]] std::uint64_t totalMicroseconds() const noexcept {
        const std::uint32_t period = subTickPeriod();
        const std::uint32_t carryMs = subTick_ / period;
        const std::uint32_t normalizedSubTick = subTick_ % period;
        return static_cast<std::uint64_t>(tickMs_ + carryMs) * 1000ULL +
               (static_cast<std::uint64_t>(normalizedSubTick) * 1000ULL /
                static_cast<std::uint64_t>(period));
    }

    [[nodiscard]] Timestamp operator-(const Timestamp& other) const noexcept {
        std::uint32_t diffTickMs = tickMs_ - other.tickMs_;
        std::uint32_t diffSubTick = 0U;
        if (subTick_ < other.subTick_) {
            diffTickMs -= 1U;
            diffSubTick = subTickPeriod() + subTick_ - other.subTick_;
        } else {
            diffSubTick = subTick_ - other.subTick_;
        }
        return {diffTickMs, diffSubTick};
    }

    [[nodiscard]] static std::uint32_t subTickPeriod() noexcept {
        static std::uint32_t subTickPeriod_ = SysTick->LOAD + 1;
        return subTickPeriod_;
    }

private:
    std::uint32_t tickMs_ = 0U;
    std::uint32_t subTick_ = 0U;
};
}

namespace EP::Component {
template <>
struct fmtter<EP::Bsp::Stm32::SysTickCLK::Timestamp> {
    static void execute(char*& buffer,
                        const EP::Bsp::Stm32::SysTickCLK::Timestamp& value,
                        const Feature& feature) {
        fmtter<std::uint64_t>::execute(buffer, value.totalMicroseconds(), feature);
    }
};
}
