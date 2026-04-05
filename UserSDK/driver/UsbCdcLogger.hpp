#pragma once

#include "../bsp/stm32/USBCDC.hpp"

#include <cstddef>
#include <cstdint>

namespace EP::Driver {

class UsbCdcLogger {
public:
    static BspStatus bind(USBCDC& cdc) noexcept {
        cdc_ = &cdc;
        return BspStatus::ok;
    }

    static void print(std::uint8_t* data, std::size_t len) noexcept {
        if (cdc_ == nullptr || data == nullptr || len == 0U) {
            return;
        }
        cdc_->transmit(data, len, transmitTimeoutMs);
    }

private:
    static inline USBCDC* cdc_ = nullptr;
    static constexpr std::uint32_t transmitTimeoutMs = 100U;
};

}
