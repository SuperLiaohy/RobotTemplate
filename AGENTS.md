# AI Agent Operations Guide (MCU SDK Project)

This file contains instructions and guidelines for AI coding agents operating within the `RobotTemplate` repository. 
This project is a modern C++20 Software Development Kit (SDK) designed for Microcontroller Units (MCUs).

## 1. Project Overview & Architecture

The core of the SDK is located in the `UserSDK/` directory, which is logically divided into four main layers:

- **`algorithm/`**: Hardware-independent mathematical and algorithmic libraries. Includes math tools (e.g., matrix/quaternion operations), control algorithms (e.g., PID), and filtering algorithms (e.g., Kalman filters).
- **`bsp/` (Board Support Package)**: Hardware abstraction layer for MCU-specific internal peripherals. Contains interfaces and implementations for UART, I2C, SPI, USB, CAN, etc.
- **`driver/`**: External hardware abstractions. Includes drivers for specific chips, sensors (IMU, encoders), displays (OLED, LCD), and motors. Drivers should rely on the `bsp` layer for communication.
- **`component/`**: Higher-level data logic and functional modules. This includes abstraction toolsets, communication protocols, state machines, and data processing pipelines.

## 2. Embedded Compilation & Build System

The project strictly uses CMake (>= 3.22) and C++20. Because it targets MCUs, strict embedded compilation flags must be adhered to:

- **No Exceptions & No RTTI**: `-fno-exceptions -fno-rtti`. Agents must NEVER use `try/catch` or `throw`.
- **Strict Warnings**: Code must compile cleanly without warnings (e.g., `-Wall -Wextra -Wpedantic`). Treat warnings as errors where appropriate.
- **Memory Constraint**: Avoid dynamic memory allocation (`new`/`malloc`, `std::vector`, etc.) in the core runtime. Prefer stack allocation, `std::array`, and fixed-size buffers.

### Build Commands
```bash
# Configure for PC/Host (testing)
cmake -B build -S . 

# Build the project
cmake --build build -j$(nproc)
```

## 3. Testing (PC/Local Environment)

Hardware-independent modules (especially in `algorithm` and `component`) MUST have unit tests that can be executed on a local PC. The project uses GoogleTest integrated via CMake.

```bash
# Run all tests
cd build && ctest --output-on-failure

# Run a specific test suite or test case
cd build && ctest -R "AlgorithmTest" -V
./build/UserSDK/tests/MatrixTests/MatrixTest --gtest_filter=*Initialization*
```

## 4. Code Style & Guidelines

- **Modern C++20**: Extensively use `constexpr` and `consteval` for compile-time computation. Use C++20 `requires` clauses (Concepts) for template metaprogramming instead of SFINAE.
- **Error Handling**: Since exceptions are disabled, use status enums, boolean return values, or `std::optional`/custom result structures to propagate errors.
- **Naming Conventions**: 
  - **Types** (Classes, Structs, Enums, Namespaces): `PascalCase` (e.g., `KalmanFilter`, `UartConfig`).
  - **Functions and Variables**: `camelCase` (e.g., `calculateOutput`, `sensorData`).
  - **Macros and Enum Values**: `UPPER_SNAKE_CASE` (e.g., `MAX_BUFFER_SIZE`, `STATUS_OK`).
- **Documentation**: Use Doxygen-style comments (`/** @brief ... */`) for all public APIs, hardware registers, and complex algorithmic steps.

## 5. Agent Workflow Mandates

1. **Verify Compilation**: Always run `cmake --build build` after generating or modifying `.cpp`/`.hpp` files to ensure zero compilation errors and zero warnings.
2. **Path Context**: Always use absolute paths starting from the workspace root (`/home/liaohy/User/Code/CLion/RobotTemplate/`).
3. **PC Testability**: When writing new logic for `algorithm` or `component`, immediately write a corresponding GTest case in the `UserSDK/tests/` directory and verify it passes locally.
4. **Hardware Separation**: Never mix MCU-specific register code or vendor HAL inside `algorithm` or `component`. Hardware specifics belong strictly in `bsp` and occasionally `driver`.
