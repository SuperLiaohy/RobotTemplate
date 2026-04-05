#pragma once

#include "../BspStatus.hpp"

#include <cstddef>
#include <cstdint>

/**
 * @brief SPI BSP abstraction with opaque hardware handle.
 */
class Spi {
public:
    /**
     * @brief Construct SPI wrapper around a platform handle.
     * @param hardwareHandle Native SPI handle pointer from platform code.
     */
    explicit Spi(void* hardwareHandle = nullptr) noexcept;
    Spi(const Spi&) = delete;
    Spi& operator=(const Spi&) = delete;
    Spi(Spi&&) = delete;
    Spi& operator=(Spi&&) = delete;

    /**
     * @brief Configure and initialize SPI peripheral.
     */
    BspStatus init() noexcept;

    /**
     * @brief Blocking transmit.
     */
    BspStatus transmit(const std::uint8_t* data, std::size_t size, std::uint32_t timeoutMs) noexcept;

    /**
     * @brief Blocking receive.
     */
    BspStatus receive(std::uint8_t* data, std::size_t size, std::uint32_t timeoutMs) noexcept;

    /**
     * @brief Blocking full-duplex transfer.
     */
    BspStatus transmitReceive(const std::uint8_t* txData, std::uint8_t* rxData, std::size_t size,
                              std::uint32_t timeoutMs) noexcept;

    /**
     * @brief Replace hardware handle used by this abstraction.
     */
    BspStatus setHardwareHandle(void* hardwareHandle) noexcept;

private:
    void* hardwareHandle_;
};
