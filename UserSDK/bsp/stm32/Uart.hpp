#pragma once

#include "../BspConcepts.hpp"
#include "../BspStatus.hpp"

#include <cstddef>
#include <cstdint>

/**
 * @brief UART BSP abstraction with opaque hardware handle.
 */
class Uart {
public:
    /**
     * @brief Construct UART wrapper around a platform handle.
     * @param hardwareHandle Native UART handle pointer from platform code.
     */
    explicit Uart(void* hardwareHandle = nullptr) noexcept;
    Uart(const Uart&) = delete;
    Uart& operator=(const Uart&) = delete;
    Uart(Uart&&) = delete;
    Uart& operator=(Uart&&) = delete;

    /**
     * @brief Configure and initialize UART peripheral.
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
     * @brief Interrupt mode transmit.
     */
    BspStatus transmitIT(const std::uint8_t* data, std::size_t size) noexcept;

    /**
     * @brief Interrupt mode receive.
     */
    BspStatus receiveIT(std::uint8_t* data, std::size_t size) noexcept;

    /**
     * @brief DMA mode transmit.
     */
    BspStatus transmitDMA(const std::uint8_t* data, std::size_t size) noexcept;

    /**
     * @brief DMA mode receive.
     */
    BspStatus receiveDMA(std::uint8_t* data, std::size_t size) noexcept;

    /**
     * @brief Interrupt mode receive-to-idle.
     */
    BspStatus receiveToIdleIT(std::uint8_t* data, std::size_t size) noexcept;

    /**
     * @brief DMA mode receive-to-idle.
     */
    BspStatus receiveToIdleDMA(std::uint8_t* data, std::size_t size) noexcept;

    /**
     * @brief Register asynchronous event callback.
     */
    BspStatus registerCallback(UartCallback callback, void* userContext) noexcept;

    /**
     * @brief Replace hardware handle and rebind instance mapping.
     */
    BspStatus setHardwareHandle(void* hardwareHandle) noexcept;

    /**
     * @brief Dispatch asynchronous UART event to registered callback.
     * @param event UART event type.
     * @param length Received length associated with event, 0 when not applicable.
     */
    void dispatchEvent(UartEvent event, std::size_t length) noexcept;

private:
    void* hardwareHandle_;
    UartCallback callback_;
    void* callbackUserContext_;
};
