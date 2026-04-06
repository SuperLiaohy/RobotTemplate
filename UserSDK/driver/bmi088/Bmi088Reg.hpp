#pragma once

#include <cstdint>

namespace EP::Driver {

enum class Bmi088AccelRange : std::uint8_t {
    g3 = 0x00U,
    g6 = 0x01U,
    g12 = 0x02U,
    g24 = 0x03U
};

enum class Bmi088AccelBandwidth : std::uint8_t {
    osr4 = 0x00U,
    osr2 = 0x01U,
    normal = 0x02U
};

enum class Bmi088AccelOdr : std::uint8_t {
    hz12_5 = 0x05U,
    hz25 = 0x06U,
    hz50 = 0x07U,
    hz100 = 0x08U,
    hz200 = 0x09U,
    hz400 = 0x0AU,
    hz800 = 0x0BU,
    hz1600 = 0x0CU
};

enum class Bmi088GyroRange : std::uint8_t {
    dps2000 = 0x00U,
    dps1000 = 0x01U,
    dps500 = 0x02U,
    dps250 = 0x03U,
    dps125 = 0x04U
};

enum class Bmi088GyroBandwidth : std::uint8_t {
    odr2000_bw532 = 0x00U,
    odr2000_bw230 = 0x01U,
    odr1000_bw116 = 0x02U,
    odr400_bw47 = 0x03U,
    odr200_bw23 = 0x04U,
    odr100_bw12 = 0x05U,
    odr200_bw64 = 0x06U,
    odr100_bw32 = 0x07U
};

enum class Bmi088IntActiveLevel : std::uint8_t {
    activeLow = 0U,
    activeHigh = 1U
};

enum class Bmi088IntOutputMode : std::uint8_t {
    pushPull = 0U,
    openDrain = 1U
};

enum class Bmi088AccelDrdyRoute : std::uint8_t {
    int1 = 0U,
    int2 = 1U
};

enum class Bmi088GyroDrdyRoute : std::uint8_t {
    int3 = 0U,
    int4 = 1U
};

struct Bmi088AccelConfig {
    Bmi088AccelOdr odr{Bmi088AccelOdr::hz200};
    Bmi088AccelBandwidth bandwidth{Bmi088AccelBandwidth::normal};
    Bmi088AccelRange range{Bmi088AccelRange::g24};
};

struct Bmi088GyroConfig {
    Bmi088GyroBandwidth bandwidth{Bmi088GyroBandwidth::odr200_bw23};
    Bmi088GyroRange range{Bmi088GyroRange::dps2000};
};

struct Bmi088Raw3Axis {
    std::int16_t x{0};
    std::int16_t y{0};
    std::int16_t z{0};
};

struct Bmi088Data {
    float x{0.0F};
    float y{0.0F};
    float z{0.0F};
};

}
