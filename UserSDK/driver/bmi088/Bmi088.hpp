#pragma once

#include "../../bsp/BspConcepts.hpp"
#include "Bmi088Reg.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace EP::Driver {

template<IsSpi TSpi, ::IsGpioOut TAccelCs, ::IsGpioOut TGyroCs, ::IsDelay TDelay>
class Bmi088 {
public:
    static constexpr std::uint8_t bmi088AccelChipId = 0x1EU;
    static constexpr std::uint8_t bmi088GyroChipId = 0x0FU;

    explicit Bmi088(TSpi& spi, TAccelCs& accelCs, TGyroCs& gyroCs) noexcept :
        spi_(spi),
        accelCs_(accelCs),
        gyroCs_(gyroCs),
        accelScale_(computeAccelScale(Bmi088AccelRange::g24)),
        gyroScale_(computeGyroScale(Bmi088GyroRange::dps2000)) {
    }

    BspStatus init() noexcept {
        BspStatus status = spi_.init();
        if (status != BspStatus::ok) { return status; }

        status = accelCs_.setHigh();
        if (status != BspStatus::ok) { return status; }

        status = gyroCs_.setHigh();
        if (status != BspStatus::ok) { return status; }

        status = writeAccelRegister(regAccelSoftReset, softResetCmd);
        if (status != BspStatus::ok) { return status; }
        delayMs(accelResetDelayMs);

        status = writeAccelRegister(regAccelPwrConf, accelPwrConfActive);
        if (status != BspStatus::ok) { return status; }
        delayMs(accelPwrConfDelayMs);

        status = writeAccelRegister(regAccelPwrCtrl, accelPowerEnable);
        if (status != BspStatus::ok) { return status; }
        delayMs(accelPowerOnDelayMs);

        status = writeGyroRegister(regGyroSoftReset, softResetCmd);
        if (status != BspStatus::ok) { return status; }
        delayMs(gyroResetDelayMs);

        status = writeGyroRegister(regGyroLpm1, gyroPowerNormal);
        if (status != BspStatus::ok) { return status; }

        return BspStatus::ok;
    }

    BspStatus configureAccel(const Bmi088AccelConfig& config) noexcept {
        const std::uint8_t accelConf =
            (static_cast<std::uint8_t>(config.bandwidth) << accelBwPos) |
            static_cast<std::uint8_t>(config.odr);

        BspStatus status = writeAccelRegister(regAccelConf, accelConf);
        if (status != BspStatus::ok) { return status; }

        status = writeAccelRegister(regAccelRange, static_cast<std::uint8_t>(config.range));
        if (status != BspStatus::ok) { return status; }

        accelScale_ = computeAccelScale(config.range);
        return BspStatus::ok;
    }

    BspStatus configureGyro(const Bmi088GyroConfig& config) noexcept {
        BspStatus status = writeGyroRegister(regGyroBandwidth, static_cast<std::uint8_t>(config.bandwidth));
        if (status != BspStatus::ok) { return status; }

        status = writeGyroRegister(regGyroRange, static_cast<std::uint8_t>(config.range));
        if (status != BspStatus::ok) { return status; }

        gyroScale_ = computeGyroScale(config.range);
        return BspStatus::ok;
    }

    BspStatus readAccelRaw(Bmi088Raw3Axis& outData) noexcept {
        std::array<std::uint8_t, 6U> data{};
        const BspStatus status = readAccelRegisters(regAccelXlsb, data.data(), data.size());
        if (status != BspStatus::ok) { return status; }

        outData.x = static_cast<std::int16_t>(static_cast<std::uint16_t>(data[1]) << 8U | data[0]);
        outData.y = static_cast<std::int16_t>(static_cast<std::uint16_t>(data[3]) << 8U | data[2]);
        outData.z = static_cast<std::int16_t>(static_cast<std::uint16_t>(data[5]) << 8U | data[4]);
        return BspStatus::ok;
    }

    BspStatus readGyroRaw(Bmi088Raw3Axis& outData) noexcept {
        std::array<std::uint8_t, 6U> data{};
        const BspStatus status = readGyroRegisters(regGyroXlsb, data.data(), data.size());
        if (status != BspStatus::ok) { return status; }

        outData.x = static_cast<std::int16_t>(static_cast<std::uint16_t>(data[1]) << 8U | data[0]);
        outData.y = static_cast<std::int16_t>(static_cast<std::uint16_t>(data[3]) << 8U | data[2]);
        outData.z = static_cast<std::int16_t>(static_cast<std::uint16_t>(data[5]) << 8U | data[4]);
        return BspStatus::ok;
    }

    BspStatus readAccel(Bmi088Data& outData) noexcept {
        Bmi088Raw3Axis raw{};
        const BspStatus status = readAccelRaw(raw);
        if (status != BspStatus::ok) { return status; }

        outData.x = static_cast<float>(raw.x) * accelScale_;
        outData.y = static_cast<float>(raw.y) * accelScale_;
        outData.z = static_cast<float>(raw.z) * accelScale_;
        return BspStatus::ok;
    }

    BspStatus readGyro(Bmi088Data& outData) noexcept {
        Bmi088Raw3Axis raw{};
        const BspStatus status = readGyroRaw(raw);
        if (status != BspStatus::ok) { return status; }

        outData.x = static_cast<float>(raw.x) * gyroScale_;
        outData.y = static_cast<float>(raw.y) * gyroScale_;
        outData.z = static_cast<float>(raw.z) * gyroScale_;
        return BspStatus::ok;
    }

    BspStatus readAccelChipId(std::uint8_t& chipId) noexcept {
        return readAccelRegister(regAccelChipId, chipId);
    }

    BspStatus readGyroChipId(std::uint8_t& chipId) noexcept {
        return readGyroRegister(regGyroChipId, chipId);
    }

    BspStatus configureAccelDrdyInterrupt(
        Bmi088AccelDrdyRoute route,
        Bmi088IntActiveLevel level,
        Bmi088IntOutputMode outputMode
    ) noexcept {
        const std::uint8_t ioReg = (route == Bmi088AccelDrdyRoute::int1) ? regAccelInt1IoConf : regAccelInt2IoConf;

        std::uint8_t ioConf = accelIntIoOutputEnableMask;
        if (level == Bmi088IntActiveLevel::activeHigh) {
            ioConf |= accelIntLevelMask;
        }
        if (outputMode == Bmi088IntOutputMode::openDrain) {
            ioConf |= accelIntOpenDrainMask;
        }

        BspStatus status = writeAccelRegister(ioReg, ioConf);
        if (status != BspStatus::ok) { return status; }

        std::uint8_t mapReg = 0U;
        status = readAccelRegister(regAccelInt1Int2MapData, mapReg);
        if (status != BspStatus::ok) { return status; }

        mapReg &= static_cast<std::uint8_t>(~(accelInt1DrdyMask | accelInt2DrdyMask));
        mapReg |= (route == Bmi088AccelDrdyRoute::int1) ? accelInt1DrdyMask : accelInt2DrdyMask;

        return writeAccelRegister(regAccelInt1Int2MapData, mapReg);
    }

    BspStatus configureGyroDrdyInterrupt(
        Bmi088GyroDrdyRoute route,
        Bmi088IntActiveLevel level,
        Bmi088IntOutputMode outputMode
    ) noexcept {
        std::uint8_t intCtrl = 0U;
        BspStatus status = readGyroRegister(regGyroIntCtrl, intCtrl);
        if (status != BspStatus::ok) { return status; }

        intCtrl |= gyroDataReadyEnableMask;
        status = writeGyroRegister(regGyroIntCtrl, intCtrl);
        if (status != BspStatus::ok) { return status; }

        std::uint8_t ioConf = 0U;
        status = readGyroRegister(regGyroInt3Int4IoConf, ioConf);
        if (status != BspStatus::ok) { return status; }

        ioConf &= static_cast<std::uint8_t>(~(gyroInt3LevelMask | gyroInt3OpenDrainMask | gyroInt4LevelMask | gyroInt4OpenDrainMask));
        if (route == Bmi088GyroDrdyRoute::int3) {
            if (level == Bmi088IntActiveLevel::activeHigh) {
                ioConf |= gyroInt3LevelMask;
            }
            if (outputMode == Bmi088IntOutputMode::openDrain) {
                ioConf |= gyroInt3OpenDrainMask;
            }
        } else {
            if (level == Bmi088IntActiveLevel::activeHigh) {
                ioConf |= gyroInt4LevelMask;
            }
            if (outputMode == Bmi088IntOutputMode::openDrain) {
                ioConf |= gyroInt4OpenDrainMask;
            }
        }

        status = writeGyroRegister(regGyroInt3Int4IoConf, ioConf);
        if (status != BspStatus::ok) { return status; }

        std::uint8_t mapReg = 0U;
        status = readGyroRegister(regGyroInt3Int4IoMap, mapReg);
        if (status != BspStatus::ok) { return status; }

        mapReg &= static_cast<std::uint8_t>(~(gyroInt3MapMask | gyroInt4MapMask));
        mapReg |= (route == Bmi088GyroDrdyRoute::int3) ? gyroInt3MapMask : gyroInt4MapMask;

        return writeGyroRegister(regGyroInt3Int4IoMap, mapReg);
    }

private:
    static constexpr std::uint32_t spiTimeoutMs = 10U;

    static constexpr std::uint8_t regReadMask = 0x80U;

    static constexpr std::uint8_t regAccelChipId = 0x00U;
    static constexpr std::uint8_t regAccelXlsb = 0x12U;
    static constexpr std::uint8_t regAccelConf = 0x40U;
    static constexpr std::uint8_t regAccelRange = 0x41U;
    static constexpr std::uint8_t regAccelInt1IoConf = 0x53U;
    static constexpr std::uint8_t regAccelInt2IoConf = 0x54U;
    static constexpr std::uint8_t regAccelInt1Int2MapData = 0x58U;
    static constexpr std::uint8_t regAccelPwrConf = 0x7CU;
    static constexpr std::uint8_t regAccelPwrCtrl = 0x7DU;
    static constexpr std::uint8_t regAccelSoftReset = 0x7EU;

    static constexpr std::uint8_t regGyroChipId = 0x00U;
    static constexpr std::uint8_t regGyroXlsb = 0x02U;
    static constexpr std::uint8_t regGyroRange = 0x0FU;
    static constexpr std::uint8_t regGyroBandwidth = 0x10U;
    static constexpr std::uint8_t regGyroLpm1 = 0x11U;
    static constexpr std::uint8_t regGyroSoftReset = 0x14U;
    static constexpr std::uint8_t regGyroIntCtrl = 0x15U;
    static constexpr std::uint8_t regGyroInt3Int4IoConf = 0x16U;
    static constexpr std::uint8_t regGyroInt3Int4IoMap = 0x18U;

    static constexpr std::uint8_t softResetCmd = 0xB6U;
    static constexpr std::uint8_t accelPwrConfActive = 0x00U;
    static constexpr std::uint8_t accelPowerEnable = 0x04U;
    static constexpr std::uint8_t gyroPowerNormal = 0x00U;

    static constexpr std::uint32_t accelResetDelayMs = 1U;
    static constexpr std::uint32_t accelPwrConfDelayMs = 1U;
    static constexpr std::uint32_t accelPowerOnDelayMs = 50U;
    static constexpr std::uint32_t gyroResetDelayMs = 30U;

    static constexpr std::uint8_t accelBwPos = 4U;

    static constexpr std::uint8_t accelIntLevelMask = 0x02U;
    static constexpr std::uint8_t accelIntOpenDrainMask = 0x04U;
    static constexpr std::uint8_t accelIntIoOutputEnableMask = 0x08U;
    static constexpr std::uint8_t accelInt1DrdyMask = 0x04U;
    static constexpr std::uint8_t accelInt2DrdyMask = 0x40U;

    static constexpr std::uint8_t gyroDataReadyEnableMask = 0x80U;
    static constexpr std::uint8_t gyroInt3LevelMask = 0x01U;
    static constexpr std::uint8_t gyroInt3OpenDrainMask = 0x02U;
    static constexpr std::uint8_t gyroInt4LevelMask = 0x04U;
    static constexpr std::uint8_t gyroInt4OpenDrainMask = 0x08U;
    static constexpr std::uint8_t gyroInt3MapMask = 0x01U;
    static constexpr std::uint8_t gyroInt4MapMask = 0x80U;

    static constexpr float lsbDenominator = 32768.0F;
    static constexpr float gravityMs2 = 9.80665F;

    static constexpr float accelRangeToG(Bmi088AccelRange range) noexcept {
        switch (range) {
            case Bmi088AccelRange::g3:
                return 3.0F;
            case Bmi088AccelRange::g6:
                return 6.0F;
            case Bmi088AccelRange::g12:
                return 12.0F;
            case Bmi088AccelRange::g24:
                return 24.0F;
        }
        return 24.0F;
    }

    static constexpr float gyroRangeToDps(Bmi088GyroRange range) noexcept {
        switch (range) {
            case Bmi088GyroRange::dps2000:
                return 2000.0F;
            case Bmi088GyroRange::dps1000:
                return 1000.0F;
            case Bmi088GyroRange::dps500:
                return 500.0F;
            case Bmi088GyroRange::dps250:
                return 250.0F;
            case Bmi088GyroRange::dps125:
                return 125.0F;
        }
        return 2000.0F;
    }

    static constexpr float computeAccelScale(Bmi088AccelRange range) noexcept {
        return (accelRangeToG(range) * gravityMs2) / lsbDenominator;
    }

    static constexpr float computeGyroScale(Bmi088GyroRange range) noexcept {
        return gyroRangeToDps(range) / lsbDenominator;
    }

    BspStatus writeAccelRegister(std::uint8_t reg, std::uint8_t value) noexcept {
        const std::array<std::uint8_t, 2U> txData = {static_cast<std::uint8_t>(reg & static_cast<std::uint8_t>(~regReadMask)), value};
        std::array<std::uint8_t, 2U> rxData{};
        return transferWithCs(accelCs_, txData.data(), rxData.data(), txData.size());
    }

    BspStatus writeGyroRegister(std::uint8_t reg, std::uint8_t value) noexcept {
        const std::array<std::uint8_t, 2U> txData = {static_cast<std::uint8_t>(reg & static_cast<std::uint8_t>(~regReadMask)), value};
        std::array<std::uint8_t, 2U> rxData{};
        return transferWithCs(gyroCs_, txData.data(), rxData.data(), txData.size());
    }

    BspStatus readAccelRegister(std::uint8_t reg, std::uint8_t& outValue) noexcept {
        std::array<std::uint8_t, 1U> data{};
        const BspStatus status = readAccelRegisters(reg, data.data(), data.size());
        if (status != BspStatus::ok) { return status; }
        outValue = data[0];
        return BspStatus::ok;
    }

    BspStatus readGyroRegister(std::uint8_t reg, std::uint8_t& outValue) noexcept {
        std::array<std::uint8_t, 1U> data{};
        const BspStatus status = readGyroRegisters(reg, data.data(), data.size());
        if (status != BspStatus::ok) { return status; }
        outValue = data[0];
        return BspStatus::ok;
    }

    BspStatus readAccelRegisters(std::uint8_t startReg, std::uint8_t* outData, std::size_t length) noexcept {
        if ((outData == nullptr) || (length == 0U)) { return BspStatus::invalidArgument; }
        if (length > 6U) { return BspStatus::invalidArgument; }

        std::array<std::uint8_t, 8U> txData{};
        std::array<std::uint8_t, 8U> rxData{};
        txData[0] = static_cast<std::uint8_t>(startReg | regReadMask);
        txData[1] = 0U;

        const BspStatus status = transferWithCs(accelCs_, txData.data(), rxData.data(), length + 2U);
        if (status != BspStatus::ok) { return status; }

        for (std::size_t i = 0U; i < length; ++i) {
            outData[i] = rxData[i + 2U];
        }
        return BspStatus::ok;
    }

    BspStatus readGyroRegisters(std::uint8_t startReg, std::uint8_t* outData, std::size_t length) noexcept {
        if ((outData == nullptr) || (length == 0U)) { return BspStatus::invalidArgument; }
        if (length > 6U) { return BspStatus::invalidArgument; }

        std::array<std::uint8_t, 7U> txData{};
        std::array<std::uint8_t, 7U> rxData{};
        txData[0] = static_cast<std::uint8_t>(startReg | regReadMask);

        const BspStatus status = transferWithCs(gyroCs_, txData.data(), rxData.data(), length + 1U);
        if (status != BspStatus::ok) { return status; }

        for (std::size_t i = 0U; i < length; ++i) {
            outData[i] = rxData[i + 1U];
        }
        return BspStatus::ok;
    }

    template<::IsGpioOut TGpioOut>
    BspStatus transferWithCs(TGpioOut& cs, const std::uint8_t* tx, std::uint8_t* rx, std::size_t size) noexcept {
        BspStatus status = cs.setLow();
        if (status != BspStatus::ok) { return status; }

        const BspStatus transferStatus = spi_.transmitReceive(tx, rx, size, spiTimeoutMs);

        status = cs.setHigh();
        if (status != BspStatus::ok) { return status; }

        return transferStatus;
    }

    void delayMs(std::uint32_t timeoutMs) noexcept {
        TDelay::halDelayMs(timeoutMs);
    }

    TSpi& spi_;
    TAccelCs& accelCs_;
    TGyroCs& gyroCs_;
    float accelScale_;
    float gyroScale_;
};

}
