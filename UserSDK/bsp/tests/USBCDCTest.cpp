#include "gtest/gtest.h"

#include "../BspConcepts.hpp"
#include "../stm32/USBCDC.hpp"

TEST(USBCDCTest, UsbCdcMeetsConceptRequirements) {
    static_assert(IsUsbCdc<USBCDC>);
}

TEST(USBCDCTest, SimulateUsbCdcSmokePath) {
    std::uint32_t hardwareToken = 0xABCDU;
    USBCDC cdc(&hardwareToken);

    EXPECT_EQ(cdc.init(), BspStatus::ok);

    std::uint8_t tx[3] = {9U, 8U, 7U};
    std::uint8_t rx[3] = {0xAAU, 0xAAU, 0xAAU};

    EXPECT_EQ(cdc.transmit(tx, 3U, 5U), BspStatus::ok);
    EXPECT_EQ(cdc.receive(rx, 3U, 5U), BspStatus::ok);
    EXPECT_EQ(rx[0], 0U);
    EXPECT_EQ(rx[1], 0U);
    EXPECT_EQ(rx[2], 0U);
}
