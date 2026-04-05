#ifdef USE_HAL_DRIVER

#include "Gpio.hpp"

extern "C" {
#include <gpio.h>
}

namespace {
GPIO_TypeDef* toHalPort(void* rawPortHandle) noexcept {
    return static_cast<GPIO_TypeDef*>(rawPortHandle);
}
}

Gpio::Gpio(void* portHandle, std::uint16_t pinMask) noexcept : portHandle_(portHandle), pinMask_(pinMask) {}

BspStatus Gpio::setHigh() noexcept {
    auto* port = toHalPort(portHandle_);
#ifndef NDEBUG
    if (port == nullptr || pinMask_ == 0U) {
        return BspStatus::invalidArgument;
    }
#endif
    HAL_GPIO_WritePin(port, pinMask_, GPIO_PIN_SET);
    return BspStatus::ok;
}

BspStatus Gpio::setLow() noexcept {
    auto* port = toHalPort(portHandle_);
#ifndef NDEBUG
    if (port == nullptr || pinMask_ == 0U) {
        return BspStatus::invalidArgument;
    }
#endif
    HAL_GPIO_WritePin(port, pinMask_, GPIO_PIN_RESET);
    return BspStatus::ok;
}

BspStatus Gpio::toggle() noexcept {
    auto* port = toHalPort(portHandle_);
#ifndef NDEBUG
    if (port == nullptr || pinMask_ == 0U) {
        return BspStatus::invalidArgument;
    }
#endif
    HAL_GPIO_TogglePin(port, pinMask_);
    return BspStatus::ok;
}

bool Gpio::read() noexcept {
    auto* port = toHalPort(portHandle_);
#ifndef NDEBUG
    if (port == nullptr || pinMask_ == 0U) {
        return false;
    }
#endif
    return HAL_GPIO_ReadPin(port, pinMask_) == GPIO_PIN_SET;
}

BspStatus Gpio::setHardwareHandle(void* portHandle, std::uint16_t pinMask) noexcept {
    portHandle_ = portHandle;
    pinMask_ = pinMask;
#ifndef NDEBUG
    return (portHandle_ == nullptr || pinMask_ == 0U) ? BspStatus::invalidArgument : BspStatus::ok;
#else
    return BspStatus::ok;
#endif
}

std::uint16_t Gpio::pinMask() const noexcept {
    return pinMask_;
}

#endif
