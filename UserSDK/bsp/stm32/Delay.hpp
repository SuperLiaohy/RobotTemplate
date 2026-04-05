#pragma once

#include <cstdint>

/**
 * @brief Delay BSP abstraction for millisecond blocking waits.
 */
class Delay {
public:
    /**
     * @brief Block current context for a number of milliseconds.
     * @param timeoutMs Delay duration in milliseconds.
     */
    void delayMs(std::uint32_t timeoutMs) noexcept;
};
