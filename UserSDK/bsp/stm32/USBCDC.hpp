#pragma once

#include "../BspStatus.hpp"

#include <cstddef>
#include <cstdint>

#include "BspConcepts.hpp"

/**
 * @brief USB CDC BSP abstraction with an opaque hardware handle.
 */
class USBCDC {
public:
    /**
     * @brief Construct USB CDC wrapper around a platform handle.
     */
    explicit USBCDC() noexcept;
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
    BspStatus transmit(const std::uint8_t* data, std::size_t size) noexcept;

    /**
     * @brief Blocking receive over USB CDC.
     */
    BspStatus registerCallback(CDCCallback callback, void* userContext) noexcept;

    CDCCallback callback_;
    void* callbackUserContext_;
};


USBCDC& CDCInstance();

inline void CDCPrint(std::uint8_t* data, std::size_t len) noexcept {
    CDCInstance().transmit(data, len);
}
