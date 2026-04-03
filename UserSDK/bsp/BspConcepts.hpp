#pragma once

#include "BspStatus.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>

/**
 * @brief UART asynchronous event type for callback dispatch.
 */
enum class UartEvent : std::uint8_t {
    txComplete = 0U,
    rxComplete,
    rxIdle,
    error
};

/**
 * @brief UART callback signature.
 */
using UartCallback = void(*)(UartEvent event, std::size_t length, void* userContext) noexcept;

template<typename T>
concept IsUart = requires(
    T uart,
    const std::uint8_t* txData,
    std::uint8_t* rxData,
    std::size_t size,
    std::uint32_t timeoutMs,
    UartCallback callback,
    void* userContext
) {
    { uart.init() } -> std::same_as<BspStatus>;
    { uart.transmit(txData, size, timeoutMs) } -> std::same_as<BspStatus>;
    { uart.receive(rxData, size, timeoutMs) } -> std::same_as<BspStatus>;
    { uart.transmitIT(txData, size) } -> std::same_as<BspStatus>;
    { uart.receiveIT(rxData, size) } -> std::same_as<BspStatus>;
    { uart.transmitDMA(txData, size) } -> std::same_as<BspStatus>;
    { uart.receiveDMA(rxData, size) } -> std::same_as<BspStatus>;
    { uart.receiveToIdleIT(rxData, size) } -> std::same_as<BspStatus>;
    { uart.receiveToIdleDMA(rxData, size) } -> std::same_as<BspStatus>;
    { uart.registerCallback(callback, userContext) } -> std::same_as<BspStatus>;
};

template<typename T>
concept IsUsbCdc = requires(
    T cdc,
    const std::uint8_t* txData,
    std::uint8_t* rxData,
    std::size_t size,
    std::uint32_t timeoutMs
) {
    { cdc.init() } -> std::same_as<BspStatus>;
    { cdc.transmit(txData, size, timeoutMs) } -> std::same_as<BspStatus>;
    { cdc.receive(rxData, size, timeoutMs) } -> std::same_as<BspStatus>;
};
