---
name: hw-debug
description: Compile, auto-flash via OpenOCD, collect serial logs with timeout, and iterate to resolve hardware bugs. Use GDB only when necessary.
category: writing
---

# hw-debug

Run a streamlined, autonomous hardware debug loop for STM32 issues. Emphasize iterative code modification and serial logging over heavy GDB sessions.

## Debug Philosophy

1. **Rely on Serial First**: Print-based debugging via serial is faster. Use it as your primary feedback loop.
2. **Limit Execution Time**: The serial logger now supports `--timeout <seconds>`. Judge an appropriate runtime (e.g., 5-10 seconds) for your test to avoid hanging the autonomous agent.
3. **Use GDB Only When Necessary**: If a hardfault occurs or you need deep register inspection, fallback to the full GDB flow.

## Standard Workflow (Serial-driven)

This flow is much faster and avoids lingering background processes.

1. **Compile the Firmware**
   ```bash
   cmake -B WorkSpace/Stm32Demo/build -S WorkSpace/Stm32Demo -DCMAKE_TOOLCHAIN_FILE=WorkSpace/Stm32Demo/cmake/starm-clang.cmake
   cmake --build WorkSpace/Stm32Demo/build
   ```

2. **Setup Session Directory**
   ```bash
   SESSION_DIR="debug_sessions/session_$(date +%Y%m%d_%H%M%S)" && mkdir -p "$SESSION_DIR"
   ```

3. **Flash and Reset via OpenOCD**
   This command programs the ELF, verifies it, resets the board to run, and immediately exits OpenOCD.
   ```bash
   openocd -f WorkSpace/Stm32Demo/stm32h7.cfg -c "program WorkSpace/Stm32Demo/build/Stm32Demo.elf verify reset exit" > "$SESSION_DIR/flash.log" 2>&1
   ```

4. **Capture Serial Logs (Auto-Port & Timeout)**
   Run the serial logger with a reasonable timeout. The script auto-detects the port (prioritizing VCP over debug interface).
   ```bash
   python3 .opencode/skills/tools/serial_logger.py --port auto --timeout 10 --output "$SESSION_DIR/serial.log"
   ```

5. **Analyze and Iterate**
   - Read `$SESSION_DIR/serial.log`.
   - Modify code to test fixes or add more print statements.
   - Repeat steps 1-5 until the issue is fixed.

---

## Fallback Workflow (GDB-driven)

*Use this ONLY if you need backtraces, memory inspection, or cannot diagnose the issue via serial logs.*

1. **Write a Tailored GDB Script**
   ```bash
   cat << 'GDB' > "$SESSION_DIR/debug.gdb"
   target extended-remote localhost:3333
   monitor reset halt
   load
   # Add breakpoints here
   continue
   GDB
   ```

2. **Start OpenOCD & Serial Logger (Background)**
   ```bash
   openocd -f WorkSpace/Stm32Demo/stm32h7.cfg > "$SESSION_DIR/openocd.log" 2>&1 &
   OPENOCD_PID=$!
   
   python3 .opencode/skills/tools/serial_logger.py --port auto --timeout 15 --output "$SESSION_DIR/serial.log" &
   LOGGER_PID=$!
   ```

3. **Run GDB (Foreground)**
   ```bash
   arm-none-eabi-gdb WorkSpace/Stm32Demo/build/Stm32Demo.elf -x "$SESSION_DIR/debug.gdb" | tee "$SESSION_DIR/gdb.log"
   ```

4. **Cleanup (CRITICAL)**
   Always ensure background processes are killed after GDB finishes!
   ```bash
   kill -9 $OPENOCD_PID
   kill -9 $LOGGER_PID
   ```

## Final Step: Documentation
When the issue is resolved, write a brief `debug_report.md` summarizing the root cause, the serial/GDB evidence, and the solution applied.
