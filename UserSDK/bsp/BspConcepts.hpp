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
enum class CDCEvent : std::uint8_t {
    rxComplete = 0U,
};

/**
 * @brief UART callback signature.
 */
using UartCallback = void(*)(UartEvent event, std::size_t length, void* userContext) noexcept;

using CDCCallback = void(*)(CDCEvent event, std::size_t length, void* userContext) noexcept;

using ExtiCallback = void(*)(void* userContext) noexcept;

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

template<typename T>
concept IsGpioOut = requires(T gpio) {
    { gpio.setHigh() } -> std::same_as<BspStatus>;
    { gpio.setLow() } -> std::same_as<BspStatus>;
    { gpio.toggle() } -> std::same_as<BspStatus>;
};

template<typename T>
concept IsGpioIn = requires(T gpio) {
    { gpio.read() } -> std::same_as<bool>;
};

template<typename T>
concept IsExti = requires(
    T exti,
    ExtiCallback callback,
    void* userContext
) {
    { exti.registerCallback(callback, userContext) } -> std::same_as<BspStatus>;
};

template<typename T>
concept IsSpi = requires(
    T spi,
    const std::uint8_t* txData,
    std::uint8_t* rxData,
    std::size_t size,
    std::uint32_t timeoutMs
) {
    { spi.transmit(txData, size, timeoutMs) } -> std::same_as<BspStatus>;
    { spi.receive(rxData, size, timeoutMs) } -> std::same_as<BspStatus>;
    { spi.transmitReceive(txData, rxData, size, timeoutMs) } -> std::same_as<BspStatus>;
    { spi.transmitIT(txData, size) } -> std::same_as<BspStatus>;
    { spi.receiveIT(rxData, size) } -> std::same_as<BspStatus>;
    { spi.transmitReceiveIT(txData, rxData, size) } -> std::same_as<BspStatus>;
    { spi.transmitDMA(txData, size) } -> std::same_as<BspStatus>;
    { spi.receiveDMA(rxData, size) } -> std::same_as<BspStatus>;
    { spi.transmitReceiveDMA(txData, rxData, size) } -> std::same_as<BspStatus>;
};

template<typename T>
concept IsDelay = requires(
    std::uint32_t timeoutMs
) {
    { T::halDelayMs(timeoutMs) } -> std::same_as<void>;
    { T::osDelayMs(timeoutMs) } -> std::same_as<void>;
    { T::delayUs(timeoutMs) } -> std::same_as<void>;
};
