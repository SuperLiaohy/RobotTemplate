#include "../stm32/USBCDC.hpp"

USBCDC::USBCDC(void* hardwareHandle) noexcept : hardwareHandle_(hardwareHandle) {}

BspStatus USBCDC::init() noexcept {
#ifndef NDEBUG
    return hardwareHandle_ == nullptr ? BspStatus::invalidArgument : BspStatus::ok;
#else
    return BspStatus::ok;
#endif
}

BspStatus USBCDC::transmit(const std::uint8_t* data, std::size_t size, std::uint32_t) noexcept {
#ifndef NDEBUG
    if (hardwareHandle_ == nullptr || data == nullptr || size == 0U) {
        return BspStatus::invalidArgument;
    }
#endif
    return BspStatus::ok;
}

BspStatus USBCDC::receive(std::uint8_t* data, std::size_t size, std::uint32_t) noexcept {
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

BspStatus USBCDC::setHardwareHandle(void* hardwareHandle) noexcept {
    hardwareHandle_ = hardwareHandle;
#ifndef NDEBUG
    return hardwareHandle_ == nullptr ? BspStatus::invalidArgument : BspStatus::ok;
#else
    return BspStatus::ok;
#endif
}
