#pragma once

#include "../BspConcepts.hpp"
#include "../BspStatus.hpp"

#include <cstdint>

/**
 * @brief EXTI BSP abstraction with callback dispatch by GPIO pin.
 */
class Exti {
public:
    /**
     * @brief Construct EXTI wrapper for one GPIO pin.
     * @param pinMask GPIO pin mask associated with this EXTI line.
     */
    explicit Exti(std::uint16_t pinMask = 0U) noexcept;
    Exti(const Exti&) = delete;
    Exti& operator=(const Exti&) = delete;
    Exti(Exti&&) = delete;
    Exti& operator=(Exti&&) = delete;

    /**
     * @brief Register callback and user context for this EXTI line.
     */
    BspStatus registerCallback(ExtiCallback callback, void* userContext) noexcept;

    /**
     * @brief Replace associated EXTI pin and rebind registry entry.
     */
    BspStatus setPin(std::uint16_t pinMask) noexcept;

    /**
     * @brief Get associated EXTI pin mask.
     */
    std::uint16_t pinMask() const noexcept;

    /**
     * @brief Dispatch callback for a HAL EXTI pin callback event.
     */
    static void dispatch(std::uint16_t pinMask) noexcept;

private:
    std::uint16_t pinMask_;
    ExtiCallback callback_;
    void* callbackUserContext_;
};
