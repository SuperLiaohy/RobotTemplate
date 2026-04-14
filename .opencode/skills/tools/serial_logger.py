#!/usr/bin/env python3
"""Serial logger with auto-port detection and timeout support."""

import argparse
import sys
import time
import os
import glob
from datetime import datetime

try:
    import serial
except ImportError:
    print("ERROR: pyserial is required. Install with: pip install pyserial", file=sys.stderr)
    sys.exit(1)


def parse_args():
    parser = argparse.ArgumentParser(description="Log serial data to a file.")
    parser.add_argument("--port", default="auto", help="Serial port (e.g. /dev/ttyUSB0 or 'auto')")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate (default: 115200)")
    parser.add_argument("--output", required=True, help="Output log file path")
    parser.add_argument("--timeout", type=int, default=0, help="Timeout in seconds (0 = run forever)")
    return parser.parse_args()


def timestamp() -> str:
    return datetime.now().isoformat(timespec="seconds")


def find_serial_port():
    ports = glob.glob('/dev/ttyACM*') + glob.glob('/dev/ttyUSB*')
    if not ports:
        return None
    # Prioritize ttyACM1 or similar if multiple exist, as ACM0 is often the debug interface,
    # but let's just return the first one that we can open.
    ports.sort(reverse=True) # Usually higher number is VCP for ST-Link (e.g., ACM1 over ACM0)
    for port in ports:
        try:
            with serial.Serial(port, 115200, timeout=0.1):
                return port
        except serial.SerialException:
            pass
    return ports[0] if ports else None


def run_logger(port: str, baud: int, output_path: str, timeout: int) -> int:
    if port == "auto":
        port = find_serial_port()
        if not port:
            print("ERROR: Could not automatically find a serial port.", file=sys.stderr)
            return 1
        print(f"Auto-detected port: {port}")

    try:
        log_file = open(output_path, "w", encoding="utf-8")
    except OSError as exc:
        print(f"ERROR: failed to open output file '{output_path}': {exc}", file=sys.stderr)
        return 1

    start_time = time.time()
    reconnect_delay = 1.0

    with log_file:
        print(f"Logging {port} at {baud} baud to {output_path} (Timeout: {timeout}s)")
        
        while True:
            if timeout > 0 and (time.time() - start_time) > timeout:
                print("\nTimeout reached. Exiting.")
                return 0

            try:
                with serial.Serial(port=port, baudrate=baud, timeout=0.5) as ser:
                    print(f"Connected to {port}")
                    while True:
                        if timeout > 0 and (time.time() - start_time) > timeout:
                            print("\nTimeout reached. Exiting.")
                            return 0
                            
                        raw = ser.readline()
                        if raw:
                            line = raw.decode("utf-8", errors="replace").rstrip("\r\n")
                            log_msg = f"[{timestamp()}] {line}"
                            print(log_msg)
                            log_file.write(log_msg + "\n")
                            log_file.flush()
            except KeyboardInterrupt:
                print("\nInterrupted by user. Exiting cleanly.")
                return 0
            except (serial.SerialException, OSError) as exc:
                if timeout > 0 and (time.time() - start_time) > timeout:
                    print("\nTimeout reached during reconnect wait. Exiting.")
                    return 0
                time.sleep(reconnect_delay)

def main():
    args = parse_args()
    return run_logger(args.port, args.baud, args.output, args.timeout)

if __name__ == "__main__":
    sys.exit(main())
