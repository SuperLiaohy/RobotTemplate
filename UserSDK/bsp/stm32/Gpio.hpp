#pragma once

#include "../BspStatus.hpp"

#include <cstdint>

namespace EP::Bsp::Stm32 {

/**
 * @brief GPIO BSP abstraction for input and output operations.
 */
class Gpio {
public:
    /**
     * @brief Construct GPIO wrapper around port and pin.
     * @param portHandle Native GPIO port pointer from platform code.
     * @param pinMask GPIO pin mask.
     */
    Gpio(void* portHandle = nullptr, std::uint16_t pinMask = 0U) noexcept;
    Gpio(const Gpio&) = delete;
    Gpio& operator=(const Gpio&) = delete;
    Gpio(Gpio&&) = delete;
    Gpio& operator=(Gpio&&) = delete;

    /**
     * @brief Set output pin to logic high.
     */
    BspStatus setHigh() noexcept;

    /**
     * @brief Set output pin to logic low.
     */
    BspStatus setLow() noexcept;

    /**
     * @brief Toggle output pin state.
     */
    BspStatus toggle() noexcept;

    /**
     * @brief Read input pin state.
     * @return true when pin is high.
     */
    bool read() noexcept;

    /**
     * @brief Replace GPIO port/pin used by this abstraction.
     */
    BspStatus setHardwareHandle(void* portHandle, std::uint16_t pinMask) noexcept;

    /**
     * @brief Get configured pin mask.
     */
    std::uint16_t pinMask() const noexcept;

private:
    void* portHandle_;
    std::uint16_t pinMask_;
};

}
