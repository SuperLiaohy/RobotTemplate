#include "../stm32/Uart.hpp"

Uart::Uart(void* hardwareHandle) noexcept
    : hardwareHandle_(hardwareHandle), callback_(nullptr), callbackUserContext_(nullptr) {}

BspStatus Uart::init() noexcept {
#ifndef NDEBUG
    return hardwareHandle_ == nullptr ? BspStatus::invalidArgument : BspStatus::ok;
#else
    return BspStatus::ok;
#endif
}

BspStatus Uart::transmit(const std::uint8_t* data, std::size_t size, std::uint32_t) noexcept {
#ifndef NDEBUG
    if (hardwareHandle_ == nullptr || data == nullptr || size == 0U) {
        return BspStatus::invalidArgument;
    }
#endif
    return BspStatus::ok;
}

BspStatus Uart::receive(std::uint8_t* data, std::size_t size, std::uint32_t) noexcept {
#ifndef NDEBUG
    if (hardwareHandle_ == nullptr || data == nullptr || size == 0U) {
        return BspStatus::invalidArgument;
    }
#endif
    for (std::size_t index = 0; index < size; ++index) {
        data[index] = 0U;
    }
    return BspStatus::ok;
}

BspStatus Uart::transmitIT(const std::uint8_t* data, std::size_t size) noexcept {
    const auto status = transmit(data, size, 0U);
    if (status == BspStatus::ok) {
        dispatchEvent(UartEvent::txComplete, 0U);
    }
    return status;
}

BspStatus Uart::receiveIT(std::uint8_t* data, std::size_t size) noexcept {
    const auto status = receive(data, size, 0U);
    if (status == BspStatus::ok) {
        dispatchEvent(UartEvent::rxComplete, 0U);
    }
    return status;
}

BspStatus Uart::transmitDMA(const std::uint8_t* data, std::size_t size) noexcept {
    return transmitIT(data, size);
}

BspStatus Uart::receiveDMA(std::uint8_t* data, std::size_t size) noexcept {
    return receiveIT(data, size);
}

BspStatus Uart::receiveToIdleIT(std::uint8_t* data, std::size_t size) noexcept {
    const auto status = receive(data, size, 0U);
    if (status == BspStatus::ok) {
        dispatchEvent(UartEvent::rxIdle, size);
    }
    return status;
}

BspStatus Uart::receiveToIdleDMA(std::uint8_t* data, std::size_t size) noexcept {
    return receiveToIdleIT(data, size);
}

BspStatus Uart::registerCallback(UartCallback callback, void* userContext) noexcept {
    callback_ = callback;
    callbackUserContext_ = userContext;
    return BspStatus::ok;
}

BspStatus Uart::setHardwareHandle(void* hardwareHandle) noexcept {
    hardwareHandle_ = hardwareHandle;
#ifndef NDEBUG
    return hardwareHandle_ == nullptr ? BspStatus::invalidArgument : BspStatus::ok;
#else
    return BspStatus::ok;
#endif
}

void Uart::dispatchEvent(UartEvent event, std::size_t length) noexcept {
    if (callback_ != nullptr) {
        callback_(event, length, callbackUserContext_);
    }
}
