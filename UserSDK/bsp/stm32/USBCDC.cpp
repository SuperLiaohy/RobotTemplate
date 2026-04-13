#ifdef USE_HAL_DRIVER

#include "USBCDC.hpp"

extern "C" {
#include <usbd_cdc_if.h>
}

namespace EP::Bsp::Stm32 {
USBCDC::USBCDC() noexcept {}

BspStatus USBCDC::init() noexcept {
    return BspStatus::ok;
}

BspStatus USBCDC::transmit(const std::uint8_t* data, std::size_t size) noexcept {
    if (CDC_Transmit_HS(const_cast<std::uint8_t*>(data), static_cast<std::uint16_t>(size)) == USBD_BUSY) {
        return BspStatus::timeout;
    }
    return BspStatus::ok;
}

BspStatus USBCDC::registerCallback(CDCCallback callback, void *userContext) noexcept {
    callback_ = callback;
    callbackUserContext_ = userContext;
}

USBCDC & CDCInstance() {
    static USBCDC usbCDC;
    return usbCDC;
}

extern "C" void pCDCCallback(uint32_t len,void *userContext) {
    if (CDCInstance().callback_ != nullptr) {
        CDCInstance().callback_(CDCEvent::rxComplete,len,userContext);
    }
}
}

#endif
