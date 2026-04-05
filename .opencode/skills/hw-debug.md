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

2. **Create a dedicated debug session directory**
   - Group all artifacts for this debug event in one folder so logs and scripts stay together.
   - Use a timestamped session by default, or replace it with a named event folder such as `debug_sessions/<event_name>`.
     ```bash
     SESSION_DIR="debug_sessions/session_$(date +%Y%m%d_%H%M%S)" && mkdir -p "$SESSION_DIR"
     ```

3. **Write a tailored GDB script**
   - Create a real `$SESSION_DIR/debug.gdb` file for the current failure mode, include actual commands, not placeholders.
   - Example baseline `debug.gdb`:
     ```gdb
     target extended-remote localhost:3333
     monitor reset halt
     load
     continue
     ```
   - Extend it as needed with breakpoints, logging, and backtrace/register/memory captures.
   - If you need useful runtime serial logs, avoid scripts that stop at a breakpoint, print values, and immediately `quit`.
   - Prefer this flow after your breakpoint actions: `continue`, then `detach`, then `quit`, so firmware keeps running and the serial logger can capture post-breakpoint behavior.

4. **Start OpenOCD in background and save PID**
   ```bash
   openocd -f WorkSpace/Stm32Demo/stm32h7.cfg > "$SESSION_DIR/openocd.log" 2>&1 &
   OPENOCD_PID=$!
   ```

5. **Discover serial device before starting logger**
   - Use `lsusb` output to distinguish the debugger interface from the Virtual COM Port (VCP). On many probes, both appear and map to different nodes, for example `ttyACM0` (debug interface) and `ttyACM1` (VCP).
   ```bash
   lsusb
   # List candidate serial ports (ttyUSB* / ttyACM*) without shell-glob errors.
   find /dev -maxdepth 1 \( -name 'ttyUSB*' -o -name 'ttyACM*' \) -print
   PORT=/dev/ttyACM0  # Replace with the discovered port for your board.
   ```

6. **Start serial logger in background and save PID**
   ```bash
   python3 -c "import serial" || pip install pyserial
   python3 .opencode/skills/tools/serial_logger.py --port "$PORT" --baud 115200 --output "$SESSION_DIR/serial.log" &
   LOGGER_PID=$!
   ```
   - If `serial_logger.py` fails to start because `serial` is missing, install `pyserial` and run the logger again.

7. **Run GDB in foreground with script**
   ```bash
   arm-none-eabi-gdb WorkSpace/Stm32Demo/build/Stm32Demo.elf -x "$SESSION_DIR/debug.gdb" | tee "$SESSION_DIR/gdb_output.log"
   # If your GDB script detached, sleep here to allow the serial logger to capture runtime output.
   sleep 5
   ```
   - Do not end the GDB phase too early when collecting serial evidence. If your script uses breakpoints for inspection, let execution continue and detach first, then sleep in bash before cleanup so logs contain enough runtime context.

8. **CRITICAL cleanup: always kill background processes using saved PIDs**
   ```bash
   kill -9 $OPENOCD_PID
   kill -9 $LOGGER_PID
   ```

9. **Read outputs**
   - All logs for this debug event are grouped under `$SESSION_DIR/`.
   - Inspect `$SESSION_DIR/serial.log` from the serial logger.
   - Inspect `$SESSION_DIR/gdb_output.log` (and `$SESSION_DIR/openocd.log` if needed).

10. **Analyze and iterate**
    - Determine root cause from logs/output.
    - Modify code based on findings.
    - Repeat this full cycle until the issue is fixed.

11. **Generate a summary report**
    - Read the collected logs from `$SESSION_DIR/serial.log`, `$SESSION_DIR/gdb_output.log`, and `$SESSION_DIR/openocd.log`.
    - Create a markdown report at `$SESSION_DIR/debug_report.md`.
    - Include these sections in the report:
      - `## Test Overview`
      - `## GDB Data Analysis` (values extracted)
      - `## Serial Logging Observations`
      - `## Root Cause / Conclusion`
    - Keep this report updated on each iteration so every debug session has a clear trace of evidence and conclusions.

## Non-negotiables

- Do not skip PID capture (`OPENOCD_PID=$!`, `LOGGER_PID=$!`).
- Do not skip forced cleanup (`kill -9 $OPENOCD_PID`, `kill -9 $LOGGER_PID`).
- Do not leave OpenOCD/logger running after a debug attempt.
