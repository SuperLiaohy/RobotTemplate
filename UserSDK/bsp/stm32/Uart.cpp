#ifdef USE_HAL_DRIVER

#include "Uart.hpp"

extern "C" {
#include <usart.h>
}

#include <array>

namespace {
constexpr std::size_t kMaxRegisteredUarts = 8U;

struct UartBinding {
    UART_HandleTypeDef* halHandle;
    Uart* instance;
};

std::array<UartBinding, kMaxRegisteredUarts> uartBindings{};

UART_HandleTypeDef* toHalHandle(void* rawHandle) noexcept {
    return static_cast<UART_HandleTypeDef*>(rawHandle);
}

HAL_StatusTypeDef transmitBlocking(UART_HandleTypeDef* handle, const std::uint8_t* data, std::uint16_t size,
                                   std::uint32_t timeoutMs) noexcept {
    return HAL_UART_Transmit(handle, const_cast<std::uint8_t*>(data), size, timeoutMs);
}

HAL_StatusTypeDef receiveBlocking(UART_HandleTypeDef* handle, std::uint8_t* data, std::uint16_t size,
                                  std::uint32_t timeoutMs) noexcept {
    return HAL_UART_Receive(handle, data, size, timeoutMs);
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

bool registerInstance(UART_HandleTypeDef* halHandle, Uart* instance) noexcept {
    for (auto& binding : uartBindings) {
        if (binding.instance == instance) {
            binding.halHandle = halHandle;
            return true;
        }
    }
    for (auto& binding : uartBindings) {
        if (binding.instance == nullptr) {
            binding.halHandle = halHandle;
            binding.instance = instance;
            return true;
        }
    }
    return false;
}

Uart* findInstance(UART_HandleTypeDef* halHandle) noexcept {
    for (auto& binding : uartBindings) {
        if (binding.halHandle == halHandle) {
            return binding.instance;
        }
    }
    return nullptr;
}
}

Uart::Uart(void* hardwareHandle) noexcept
    : hardwareHandle_(hardwareHandle), callback_(nullptr), callbackUserContext_(nullptr) {
    static_cast<void>(setHardwareHandle(hardwareHandle));
}

BspStatus Uart::init() noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_UART_Init(halHandle));
}

BspStatus Uart::transmit(const std::uint8_t* data, std::size_t size, std::uint32_t timeoutMs) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(transmitBlocking(halHandle, data, static_cast<std::uint16_t>(size), timeoutMs));
}

BspStatus Uart::receive(std::uint8_t* data, std::size_t size, std::uint32_t timeoutMs) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(receiveBlocking(halHandle, data, static_cast<std::uint16_t>(size), timeoutMs));
}

BspStatus Uart::transmitIT(const std::uint8_t* data, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_UART_Transmit_IT(halHandle, const_cast<std::uint8_t*>(data),
                                             static_cast<std::uint16_t>(size)));
}

BspStatus Uart::receiveIT(std::uint8_t* data, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_UART_Receive_IT(halHandle, data, static_cast<std::uint16_t>(size)));
}

BspStatus Uart::transmitDMA(const std::uint8_t* data, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_UART_Transmit_DMA(halHandle, const_cast<std::uint8_t*>(data),
                                              static_cast<std::uint16_t>(size)));
}

BspStatus Uart::receiveDMA(std::uint8_t* data, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_UART_Receive_DMA(halHandle, data, static_cast<std::uint16_t>(size)));
}

BspStatus Uart::receiveToIdleIT(std::uint8_t* data, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_UARTEx_ReceiveToIdle_IT(halHandle, data, static_cast<std::uint16_t>(size)));
}

BspStatus Uart::receiveToIdleDMA(std::uint8_t* data, std::size_t size) noexcept {
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr || data == nullptr || size == 0U || size > 0xFFFFU) {
        return BspStatus::invalidArgument;
    }
#endif
    return mapHalStatus(HAL_UARTEx_ReceiveToIdle_DMA(halHandle, data, static_cast<std::uint16_t>(size)));
}

BspStatus Uart::registerCallback(UartCallback callback, void* userContext) noexcept {
    callback_ = callback;
    callbackUserContext_ = userContext;
    return BspStatus::ok;
}

BspStatus Uart::setHardwareHandle(void* hardwareHandle) noexcept {
    hardwareHandle_ = hardwareHandle;
    auto* halHandle = toHalHandle(hardwareHandle_);
#ifndef NDEBUG
    if (halHandle == nullptr) {
        return BspStatus::invalidArgument;
    }
#endif
    if (!registerInstance(halHandle, this)) {
        return BspStatus::busy;
    }
    return BspStatus::ok;
}

void Uart::dispatchEvent(UartEvent event, std::size_t length) noexcept {
    if (callback_ != nullptr) {
        callback_(event, length, callbackUserContext_);
    }
}

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart) {
    auto* instance = findInstance(huart);
    if (instance != nullptr) {
        instance->dispatchEvent(UartEvent::txComplete, 0U);
    }
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    auto* instance = findInstance(huart);
    if (instance != nullptr) {
        instance->dispatchEvent(UartEvent::rxComplete, 0U);
    }
}

extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size) {
    auto* instance = findInstance(huart);
    if (instance != nullptr) {
        instance->dispatchEvent(UartEvent::rxIdle, static_cast<std::size_t>(Size));
    }
}

extern "C" void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
    auto* instance = findInstance(huart);
    if (instance != nullptr) {
        instance->dispatchEvent(UartEvent::error, 0U);
    }
}

#endif
