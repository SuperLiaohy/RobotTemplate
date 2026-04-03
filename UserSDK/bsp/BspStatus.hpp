#pragma once

#include <cstdint>

/**
 * @brief Shared return status for BSP interfaces.
 */
enum class BspStatus : std::uint8_t {
    ok = 0U,
    invalidArgument,
    notInitialized,
    busy,
    timeout,
    hardwareError,
    unsupported
};
