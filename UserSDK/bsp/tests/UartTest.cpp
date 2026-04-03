#include "gtest/gtest.h"

#include "../BspConcepts.hpp"
#include "../stm32/Uart.hpp"

namespace {
static UartEvent gLastEvent = UartEvent::error;
static std::size_t gLastLength = 0U;
static void* gLastUserContext = nullptr;

void testCallback(UartEvent event, std::size_t length, void* userContext) noexcept {
    gLastEvent = event;
    gLastLength = length;
    gLastUserContext = userContext;
}
}

TEST(UartTest, UartMeetsConceptRequirements) {
    static_assert(IsUart<Uart>);
}

TEST(UartTest, SimulateUartSmokePath) {
    std::uint32_t hardwareToken = 0x1234U;
    Uart uart(&hardwareToken);

    EXPECT_EQ(uart.init(), BspStatus::ok);

    std::uint8_t tx[4] = {1U, 2U, 3U, 4U};
    std::uint8_t rx[4] = {0xFFU, 0xFFU, 0xFFU, 0xFFU};

    EXPECT_EQ(uart.registerCallback(testCallback, &hardwareToken), BspStatus::ok);

    EXPECT_EQ(uart.transmit(tx, 4U, 10U), BspStatus::ok);
    EXPECT_EQ(uart.receive(rx, 4U, 10U), BspStatus::ok);
    EXPECT_EQ(uart.transmitIT(tx, 4U), BspStatus::ok);
    EXPECT_EQ(gLastEvent, UartEvent::txComplete);
    EXPECT_EQ(gLastLength, 0U);
    EXPECT_EQ(gLastUserContext, &hardwareToken);

    EXPECT_EQ(uart.receiveIT(rx, 4U), BspStatus::ok);
    EXPECT_EQ(gLastEvent, UartEvent::rxComplete);
    EXPECT_EQ(gLastLength, 0U);

    EXPECT_EQ(uart.transmitDMA(tx, 4U), BspStatus::ok);
    EXPECT_EQ(uart.receiveDMA(rx, 4U), BspStatus::ok);

    EXPECT_EQ(uart.receiveToIdleIT(rx, 4U), BspStatus::ok);
    EXPECT_EQ(gLastEvent, UartEvent::rxIdle);
    EXPECT_EQ(gLastLength, 4U);

    EXPECT_EQ(uart.receiveToIdleDMA(rx, 3U), BspStatus::ok);
    EXPECT_EQ(gLastEvent, UartEvent::rxIdle);
    EXPECT_EQ(gLastLength, 3U);
}
