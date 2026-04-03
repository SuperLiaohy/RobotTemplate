#pragma once

#include "../BspStatus.hpp"

#include <cstddef>
#include <cstdint>

/**
 * @brief USB CDC BSP abstraction with an opaque hardware handle.
 */
class USBCDC {
public:
    /**
     * @brief Construct USB CDC wrapper around a platform handle.
     */
    explicit USBCDC(void* hardwareHandle = nullptr) noexcept;
    USBCDC(const USBCDC&) = delete;
    USBCDC& operator=(const USBCDC&) = delete;
    USBCDC(USBCDC&&) = delete;
    USBCDC& operator=(USBCDC&&) = delete;

    /**
     * @brief Configure and initialize USB CDC peripheral.
     */
    BspStatus init() noexcept;

    /**
     * @brief Blocking transmit over USB CDC.
     */
    BspStatus transmit(const std::uint8_t* data, std::size_t size, std::uint32_t timeoutMs) noexcept;

    /**
     * @brief Blocking receive over USB CDC.
     */
    BspStatus receive(std::uint8_t* data, std::size_t size, std::uint32_t timeoutMs) noexcept;

    /**
     * @brief Replace hardware handle used by this abstraction.
     */
    BspStatus setHardwareHandle(void* hardwareHandle) noexcept;

private:
    void* hardwareHandle_;
};
