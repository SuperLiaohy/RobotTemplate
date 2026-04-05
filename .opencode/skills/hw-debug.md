---
name: hw-debug
description: Compile, flash-debug, collect logs, and iterate until hardware issue is resolved.
category: writing
---

# hw-debug

Run a strict hardware debug loop for STM32 issues.

## Workflow

1. **Pre-flight checks**
   - Configure and build firmware first:
     ```bash
     cmake -B WorkSpace/Stm32Demo/build -S WorkSpace/Stm32Demo -DCMAKE_TOOLCHAIN_FILE=WorkSpace/Stm32Demo/cmake/starm-clang.cmake
     cmake --build WorkSpace/Stm32Demo/build
     ```
   - Ensure OpenOCD config exists and is readable:
     ```bash
     test -r WorkSpace/Stm32Demo/stm32h7.cfg
     ```

2. **Write a tailored GDB script**
   - Create `debug.gdb` for the current failure mode (breakpoints, monitor commands, logging, run/continue, backtrace/register/memory captures as needed).

3. **Start OpenOCD in background and save PID**
   ```bash
   openocd -f WorkSpace/Stm32Demo/stm32h7.cfg > openocd.log 2>&1 &
   OPENOCD_PID=$!
   ```

4. **Discover serial device before starting logger**
   ```bash
   lsusb
   # List candidate serial ports (ttyUSB* / ttyACM*) without shell-glob errors.
   find /dev -maxdepth 1 \( -name 'ttyUSB*' -o -name 'ttyACM*' \) -print
   PORT=/dev/ttyACM0  # Replace with the discovered port for your board.
   ```

5. **Start serial logger in background and save PID**
   ```bash
   python3 tools/serial_logger.py --port "$PORT" --baud 115200 --output serial.log &
   LOGGER_PID=$!
   ```

6. **Run GDB in foreground with script**
   ```bash
   arm-none-eabi-gdb WorkSpace/Stm32Demo/build/Stm32Demo.elf -x debug.gdb | tee gdb_output.log
   ```

7. **CRITICAL cleanup: always kill background processes using saved PIDs**
   ```bash
   kill -9 $OPENOCD_PID
   kill -9 $LOGGER_PID
   ```

8. **Read outputs**
   - Inspect `serial.log` from the serial logger.
   - Inspect `gdb_output.log` (and `openocd.log` if needed).

9. **Analyze and iterate**
   - Determine root cause from logs/output.
   - Modify code based on findings.
   - Repeat this full cycle until the issue is fixed.

## Non-negotiables

- Do not skip PID capture (`OPENOCD_PID=$!`, `LOGGER_PID=$!`).
- Do not skip forced cleanup (`kill -9 $OPENOCD_PID`, `kill -9 $LOGGER_PID`).
- Do not leave OpenOCD/logger running after a debug attempt.
