#ifdef USE_HAL_DRIVER

#include "Exti.hpp"

#include <array>

namespace EP::Bsp::Stm32 {

namespace {
constexpr std::size_t kMaxRegisteredExti = 16U;

struct ExtiBinding {
    std::uint16_t pinMask;
    Exti* instance;
};

std::array<ExtiBinding, kMaxRegisteredExti> extiBindings{};

bool registerInstance(std::uint16_t pinMask, Exti* instance) noexcept {
    for (auto& binding : extiBindings) {
        if (binding.instance == instance) {
            binding.pinMask = pinMask;
            return true;
        }
    }

    for (auto& binding : extiBindings) {
        if (binding.instance == nullptr) {
            binding.pinMask = pinMask;
            binding.instance = instance;
            return true;
        }
    }

    return false;
}

Exti* findInstance(std::uint16_t pinMask) noexcept {
    for (auto& binding : extiBindings) {
        if (binding.pinMask == pinMask) {
            return binding.instance;
        }
    }
    return nullptr;
}
}

Exti::Exti(std::uint16_t pinMask) noexcept : pinMask_(pinMask), callback_(nullptr), callbackUserContext_(nullptr) {
    static_cast<void>(setPin(pinMask));
}

BspStatus Exti::registerCallback(ExtiCallback callback, void* userContext) noexcept {
#ifndef NDEBUG
    if (callback == nullptr) {
        return BspStatus::invalidArgument;
    }
#endif
    callback_ = callback;
    callbackUserContext_ = userContext;
    return BspStatus::ok;
}

BspStatus Exti::setPin(std::uint16_t pinMask) noexcept {
    pinMask_ = pinMask;
#ifndef NDEBUG
    if (pinMask_ == 0U) {
        return BspStatus::invalidArgument;
    }
#endif
    if (!registerInstance(pinMask_, this)) {
        return BspStatus::busy;
    }
    return BspStatus::ok;
}

std::uint16_t Exti::pinMask() const noexcept {
    return pinMask_;
}

void Exti::dispatch(std::uint16_t pinMask) noexcept {
    auto* instance = findInstance(pinMask);
    if (instance != nullptr && instance->callback_ != nullptr) {
        instance->callback_(instance->callbackUserContext_);
    }
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    Exti::dispatch(GPIO_Pin);
}

}

#endif
