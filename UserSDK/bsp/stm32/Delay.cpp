#ifdef USE_HAL_DRIVER

#include "Delay.hpp"

extern "C" void HAL_Delay(uint32_t Delay);

void Delay::delayMs(std::uint32_t timeoutMs) noexcept {
    HAL_Delay(timeoutMs);
}

#endif
