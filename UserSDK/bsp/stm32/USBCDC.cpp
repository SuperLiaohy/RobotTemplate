#ifdef USE_HAL_DRIVER

#include "USBCDC.hpp"

extern "C" {
#include <usbd_cdc_if.h>
}

USBCDC::USBCDC(void* hardwareHandle) noexcept : hardwareHandle_(hardwareHandle) {}

BspStatus USBCDC::init() noexcept {
#ifndef NDEBUG
    if (hardwareHandle_ == nullptr) {
        return BspStatus::invalidArgument;
    }
#endif
    return BspStatus::ok;
}

BspStatus USBCDC::transmit(const std::uint8_t* data, std::size_t size, std::uint32_t timeoutMs) noexcept {
#ifndef NDEBUG
    if (hardwareHandle_ == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif

    std::uint32_t remaining = timeoutMs;
    while (CDC_Transmit_HS(const_cast<std::uint8_t*>(data), static_cast<std::uint16_t>(size)) == USBD_BUSY) {
        if (remaining == 0U) {
            return BspStatus::timeout;
        }
        --remaining;
    }
    return BspStatus::ok;
}

BspStatus USBCDC::receive(std::uint8_t* data, std::size_t size, std::uint32_t) noexcept {
#ifndef NDEBUG
    if (hardwareHandle_ == nullptr || data == nullptr || size == 0U) {
        return BspStatus::invalidArgument;
    }
#endif
    return BspStatus::unsupported;
}

BspStatus USBCDC::setHardwareHandle(void* hardwareHandle) noexcept {
    hardwareHandle_ = hardwareHandle;
#ifndef NDEBUG
    return hardwareHandle_ == nullptr ? BspStatus::invalidArgument : BspStatus::ok;
#else
    return BspStatus::ok;
#endif
}

#endif
