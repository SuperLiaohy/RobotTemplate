#include <gtest/gtest.h>

#include "bmi088/Bmi088.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cmath>

namespace {

enum class ActiveCs : std::uint8_t {
    none = 0U,
    accel,
    gyro
};

struct BusContext {
    ActiveCs activeCs{ActiveCs::none};
};

struct MockSpi {
    BspStatus init() noexcept {
        initCallCount++;
        return BspStatus::ok;
    }

    BspStatus transmit(const std::uint8_t*, std::size_t, std::uint32_t) noexcept {
        return BspStatus::unsupported;
    }

    BspStatus receive(std::uint8_t*, std::size_t, std::uint32_t) noexcept {
        return BspStatus::unsupported;
    }

    BspStatus transmitReceive(const std::uint8_t* txData, std::uint8_t* rxData, std::size_t size, std::uint32_t) noexcept {
        lastTransferSize = size;
        firstTxByte = txData[0];
        secondTxByte = (size > 1U) ? txData[1] : 0U;
        transferCount++;

        if (shouldFillAccelRead && (context != nullptr) && (context->activeCs == ActiveCs::accel) && (size == 8U)) {
            for (std::size_t i = 0U; i < size; ++i) {
                rxData[i] = 0U;
            }
            for (std::size_t i = 0U; i < accelReadPayload.size(); ++i) {
                rxData[i + 2U] = accelReadPayload[i];
            }
        }

        if (shouldFillGyroRead && (context != nullptr) && (context->activeCs == ActiveCs::gyro) && (size == 7U)) {
            for (std::size_t i = 0U; i < size; ++i) {
                rxData[i] = 0U;
            }
            for (std::size_t i = 0U; i < gyroReadPayload.size(); ++i) {
                rxData[i + 1U] = gyroReadPayload[i];
            }
        }

        return BspStatus::ok;
    }

    BusContext* context{nullptr};
    bool shouldFillAccelRead{false};
    bool shouldFillGyroRead{false};
    std::array<std::uint8_t, 6U> accelReadPayload{0x34U, 0x12U, 0x78U, 0x56U, 0xBCU, 0x9AU};
    std::array<std::uint8_t, 6U> gyroReadPayload{0x21U, 0x43U, 0x65U, 0x87U, 0xA9U, 0xCBU};
    std::size_t initCallCount{0U};
    std::size_t transferCount{0U};
    std::size_t lastTransferSize{0U};
    std::uint8_t firstTxByte{0U};
    std::uint8_t secondTxByte{0U};
};

struct MockAccelCs {
    explicit MockAccelCs(BusContext& ctx) noexcept : context(ctx) {
    }

    BspStatus setHigh() noexcept {
        context.activeCs = ActiveCs::none;
        return BspStatus::ok;
    }

    BspStatus setLow() noexcept {
        context.activeCs = ActiveCs::accel;
        return BspStatus::ok;
    }

    BspStatus toggle() noexcept {
        return BspStatus::ok;
    }

    BusContext& context;
};

struct MockGyroCs {
    explicit MockGyroCs(BusContext& ctx) noexcept : context(ctx) {
    }

    BspStatus setHigh() noexcept {
        context.activeCs = ActiveCs::none;
        return BspStatus::ok;
    }

    BspStatus setLow() noexcept {
        context.activeCs = ActiveCs::gyro;
        return BspStatus::ok;
    }

    BspStatus toggle() noexcept {
        return BspStatus::ok;
    }

    BusContext& context;
};

struct MockDelay {
    void delayMs(std::uint32_t timeoutMs) noexcept {
        if (callCount < calls.size()) {
            calls[callCount] = timeoutMs;
        }
        callCount++;
    }

    std::array<std::uint32_t, 8U> calls{};
    std::size_t callCount{0U};
};

}

TEST(Bmi088InitTest, AppliesRequiredDelaySequence) {
    BusContext ctx{};
    MockSpi spi{};
    spi.context = &ctx;
    MockAccelCs accelCs{ctx};
    MockGyroCs gyroCs{ctx};
    MockDelay delay{};

    EP::Driver::Bmi088<MockSpi, MockAccelCs, MockGyroCs, MockDelay> bmi(spi, accelCs, gyroCs, delay);

    const BspStatus status = bmi.init();
    EXPECT_EQ(status, BspStatus::ok);
    ASSERT_EQ(delay.callCount, 4U);
    EXPECT_EQ(delay.calls[0], 1U);
    EXPECT_EQ(delay.calls[1], 1U);
    EXPECT_EQ(delay.calls[2], 50U);
    EXPECT_EQ(delay.calls[3], 30U);
}

TEST(Bmi088ReadTest, AccelReadUsesDummyByteAndParsesDataFromOffsetTwo) {
    BusContext ctx{};
    MockSpi spi{};
    spi.context = &ctx;
    spi.shouldFillAccelRead = true;
    MockAccelCs accelCs{ctx};
    MockGyroCs gyroCs{ctx};
    MockDelay delay{};

    EP::Driver::Bmi088<MockSpi, MockAccelCs, MockGyroCs, MockDelay> bmi(spi, accelCs, gyroCs, delay);

    EP::Driver::Bmi088Raw3Axis raw{};
    const BspStatus status = bmi.readAccelRaw(raw);
    EXPECT_EQ(status, BspStatus::ok);
    EXPECT_EQ(spi.lastTransferSize, 8U);
    EXPECT_EQ(spi.firstTxByte, 0x92U);
    EXPECT_EQ(spi.secondTxByte, 0x00U);
    EXPECT_EQ(raw.x, static_cast<std::int16_t>(0x1234));
    EXPECT_EQ(raw.y, static_cast<std::int16_t>(0x5678));
    EXPECT_EQ(raw.z, static_cast<std::int16_t>(0x9ABC));
}

TEST(Bmi088ReadTest, GyroReadKeepsStandardSingleDummyPath) {
    BusContext ctx{};
    MockSpi spi{};
    spi.context = &ctx;
    spi.shouldFillGyroRead = true;
    MockAccelCs accelCs{ctx};
    MockGyroCs gyroCs{ctx};
    MockDelay delay{};

    EP::Driver::Bmi088<MockSpi, MockAccelCs, MockGyroCs, MockDelay> bmi(spi, accelCs, gyroCs, delay);

    EP::Driver::Bmi088Raw3Axis raw{};
    const BspStatus status = bmi.readGyroRaw(raw);
    EXPECT_EQ(status, BspStatus::ok);
    EXPECT_EQ(spi.lastTransferSize, 7U);
    EXPECT_EQ(spi.firstTxByte, 0x82U);
    EXPECT_EQ(raw.x, static_cast<std::int16_t>(0x4321));
    EXPECT_EQ(raw.y, static_cast<std::int16_t>(0x8765));
    EXPECT_EQ(raw.z, static_cast<std::int16_t>(0xCBA9));
}

TEST(Bmi088ReadScaledTest, AccelScalingUsesConfiguredRangeInMs2AndPreservesSign) {
    BusContext ctx{};
    MockSpi spi{};
    spi.context = &ctx;
    spi.shouldFillAccelRead = true;
    spi.accelReadPayload = {0x00U, 0x40U, 0x00U, 0xC0U, 0x00U, 0x00U};
    MockAccelCs accelCs{ctx};
    MockGyroCs gyroCs{ctx};
    MockDelay delay{};

    EP::Driver::Bmi088<MockSpi, MockAccelCs, MockGyroCs, MockDelay> bmi(spi, accelCs, gyroCs, delay);

    EP::Driver::Bmi088AccelConfig accelConfig{};
    accelConfig.range = EP::Driver::Bmi088AccelRange::g6;
    ASSERT_EQ(bmi.configureAccel(accelConfig), BspStatus::ok);

    EP::Driver::Bmi088Data data{};
    ASSERT_EQ(bmi.readAccel(data), BspStatus::ok);

    constexpr float scale = (6.0F * 9.80665F) / 32768.0F;
    EXPECT_NEAR(data.x, static_cast<float>(16384) * scale, 1e-5F);
    EXPECT_NEAR(data.y, static_cast<float>(-16384) * scale, 1e-5F);
    EXPECT_NEAR(data.z, 0.0F, 1e-6F);
}

TEST(Bmi088ReadScaledTest, GyroScalingUsesConfiguredRangeInDpsAndPreservesSign) {
    BusContext ctx{};
    MockSpi spi{};
    spi.context = &ctx;
    spi.shouldFillGyroRead = true;
    spi.gyroReadPayload = {0x00U, 0x40U, 0x00U, 0xC0U, 0x00U, 0x20U};
    MockAccelCs accelCs{ctx};
    MockGyroCs gyroCs{ctx};
    MockDelay delay{};

    EP::Driver::Bmi088<MockSpi, MockAccelCs, MockGyroCs, MockDelay> bmi(spi, accelCs, gyroCs, delay);

    EP::Driver::Bmi088GyroConfig gyroConfig{};
    gyroConfig.range = EP::Driver::Bmi088GyroRange::dps500;
    ASSERT_EQ(bmi.configureGyro(gyroConfig), BspStatus::ok);

    EP::Driver::Bmi088Data data{};
    ASSERT_EQ(bmi.readGyro(data), BspStatus::ok);

    constexpr float scale = 500.0F / 32768.0F;
    EXPECT_NEAR(data.x, static_cast<float>(16384) * scale, 1e-5F);
    EXPECT_NEAR(data.y, static_cast<float>(-16384) * scale, 1e-5F);
    EXPECT_NEAR(data.z, static_cast<float>(8192) * scale, 1e-5F);
}

TEST(Bmi088ReadScaledTest, AccelScalingAtInt16LimitsMatchesExpectedMs2) {
    BusContext ctx{};
    MockSpi spi{};
    spi.context = &ctx;
    spi.shouldFillAccelRead = true;
    spi.accelReadPayload = {0x00U, 0x80U, 0xFFU, 0x7FU, 0x00U, 0x80U};
    MockAccelCs accelCs{ctx};
    MockGyroCs gyroCs{ctx};
    MockDelay delay{};

    EP::Driver::Bmi088<MockSpi, MockAccelCs, MockGyroCs, MockDelay> bmi(spi, accelCs, gyroCs, delay);

    EP::Driver::Bmi088AccelConfig accelConfig{};
    accelConfig.range = EP::Driver::Bmi088AccelRange::g24;
    ASSERT_EQ(bmi.configureAccel(accelConfig), BspStatus::ok);

    EP::Driver::Bmi088Data data{};
    ASSERT_EQ(bmi.readAccel(data), BspStatus::ok);

    constexpr float scale = (24.0F * 9.80665F) / 32768.0F;
    EXPECT_NEAR(data.x, static_cast<float>(-32768) * scale, 1e-4F);
    EXPECT_NEAR(data.y, static_cast<float>(32767) * scale, 1e-4F);
    EXPECT_NEAR(data.z, static_cast<float>(-32768) * scale, 1e-4F);
}

TEST(Bmi088ReadScaledTest, GyroScalingAtInt16LimitsMatchesExpectedDps) {
    BusContext ctx{};
    MockSpi spi{};
    spi.context = &ctx;
    spi.shouldFillGyroRead = true;
    spi.gyroReadPayload = {0x00U, 0x80U, 0xFFU, 0x7FU, 0x00U, 0x80U};
    MockAccelCs accelCs{ctx};
    MockGyroCs gyroCs{ctx};
    MockDelay delay{};

    EP::Driver::Bmi088<MockSpi, MockAccelCs, MockGyroCs, MockDelay> bmi(spi, accelCs, gyroCs, delay);

    EP::Driver::Bmi088GyroConfig gyroConfig{};
    gyroConfig.range = EP::Driver::Bmi088GyroRange::dps2000;
    ASSERT_EQ(bmi.configureGyro(gyroConfig), BspStatus::ok);

    EP::Driver::Bmi088Data data{};
    ASSERT_EQ(bmi.readGyro(data), BspStatus::ok);

    constexpr float scale = 2000.0F / 32768.0F;
    EXPECT_NEAR(data.x, static_cast<float>(-32768) * scale, 1e-4F);
    EXPECT_NEAR(data.y, static_cast<float>(32767) * scale, 1e-4F);
    EXPECT_NEAR(data.z, static_cast<float>(-32768) * scale, 1e-4F);
}
