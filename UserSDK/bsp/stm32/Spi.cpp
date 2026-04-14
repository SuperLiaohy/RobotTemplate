#ifdef USE_HAL_DRIVER

#include "Spi.hpp"

extern "C" {
#include <spi.h>
}

namespace EP::Bsp::Stm32 {

namespace {
SPI_HandleTypeDef* toHalHandle(void* rawHandle) noexcept {
    return static_cast<SPI_HandleTypeDef*>(rawHandle);
}

BspStatus mapHalStatus(HAL_StatusTypeDef status) noexcept {
    switch (status) {
        case HAL_OK:
            return BspStatus::ok;
        case HAL_ERROR:
            return BspStatus::hardwareError;
        case HAL_BUSY:
            return BspStatus::busy;
        case HAL_TIMEOUT:
            return BspStatus::timeout;
        default:
            return BspStatus::hardwareError;
    }
}
}

Spi::Spi(void* hardwareHandle) noexcept : hardwareHandle_(hardwareHandle) {}

BspStatus Spi::init() noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_SPI_Init(halHandle));
}

BspStatus Spi::transmit(const std::uint8_t* data, std::size_t size, std::uint32_t timeoutMs) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(
        HAL_SPI_Transmit(halHandle, const_cast<std::uint8_t*>(data), static_cast<std::uint16_t>(size), timeoutMs));
}

BspStatus Spi::receive(std::uint8_t* data, std::size_t size, std::uint32_t timeoutMs) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_SPI_Receive(halHandle, data, static_cast<std::uint16_t>(size), timeoutMs));
}

BspStatus Spi::transmitReceive(const std::uint8_t* txData, std::uint8_t* rxData, std::size_t size,
                               std::uint32_t timeoutMs) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || txData == nullptr || rxData == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_SPI_TransmitReceive(halHandle, const_cast<std::uint8_t*>(txData), rxData,
                                                static_cast<std::uint16_t>(size), timeoutMs));
}

BspStatus Spi::transmitIT(const std::uint8_t* data, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_SPI_Transmit_IT(halHandle, const_cast<std::uint8_t*>(data), static_cast<std::uint16_t>(size)));
}

BspStatus Spi::receiveIT(std::uint8_t* data, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_SPI_Receive_IT(halHandle, data, static_cast<std::uint16_t>(size)));
}

BspStatus Spi::transmitReceiveIT(const std::uint8_t* txData, std::uint8_t* rxData, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || txData == nullptr || rxData == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_SPI_TransmitReceive_IT(halHandle, const_cast<std::uint8_t*>(txData), rxData,
                                                   static_cast<std::uint16_t>(size)));
}

BspStatus Spi::transmitDMA(const std::uint8_t* data, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(
        HAL_SPI_Transmit_DMA(halHandle, const_cast<std::uint8_t*>(data), static_cast<std::uint16_t>(size)));
}

BspStatus Spi::receiveDMA(std::uint8_t* data, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_SPI_Receive_DMA(halHandle, data, static_cast<std::uint16_t>(size)));
}

BspStatus Spi::transmitReceiveDMA(const std::uint8_t* txData, std::uint8_t* rxData, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || txData == nullptr || rxData == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_SPI_TransmitReceive_DMA(halHandle, const_cast<std::uint8_t*>(txData), rxData,
                                                    static_cast<std::uint16_t>(size)));
}

BspStatus Spi::setHardwareHandle(void* hardwareHandle) noexcept {
    hardwareHandle_ = hardwareHandle;
#ifndef NDEBUG
    return hardwareHandle_ == nullptr ? BspStatus::invalidArgument : BspStatus::ok;
#else
    return BspStatus::ok;
#endif
}

}

#endif
