#!/usr/bin/env python3
"""Simple serial logger with reconnect support."""

from __future__ import annotations

import argparse
import importlib
import sys
import time
from datetime import datetime


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Log serial data to a file with timestamps."
    )
    parser.add_argument(
        "--port", required=True, help="Serial port path (e.g. /dev/ttyUSB0)"
    )
    parser.add_argument(
        "--baud", type=int, default=115200, help="Baud rate (default: 115200)"
    )
    parser.add_argument("--output", required=True, help="Output log file path")
    return parser.parse_args()


def timestamp() -> str:
    return datetime.now().isoformat(timespec="seconds")


def run_logger(port: str, baud: int, output_path: str) -> int:
    reconnect_delay_seconds = 1.0

    try:
        serial_module = importlib.import_module("serial")
    except ModuleNotFoundError:
        print(
            "ERROR: pyserial is required. Install with: pip install pyserial",
            file=sys.stderr,
        )
        return 1

    try:
        log_file = open(output_path, "a", encoding="utf-8")
    except OSError as exc:
        print(
            f"ERROR: failed to open output file '{output_path}': {exc}", file=sys.stderr
        )
        return 1

    with log_file:
        print(f"Logging serial data from {port} at {baud} baud to {output_path}")
        print("Press Ctrl+C to stop.")

        while True:
            try:
                with serial_module.Serial(port=port, baudrate=baud, timeout=1) as ser:
                    print(f"Connected to serial port: {port}")

                    while True:
                        raw = ser.readline()
                        if not raw:
                            continue

                        line = raw.decode("utf-8", errors="replace").rstrip("\r\n")
                        log_file.write(f"[{timestamp()}] {line}\n")
                        log_file.flush()

            except KeyboardInterrupt:
                print("\nInterrupted by user. Exiting cleanly.")
                return 0
            except (serial_module.SerialException, OSError) as exc:
                print(
                    f"Serial connection error: {exc}. "
                    f"Retrying in {reconnect_delay_seconds:.1f}s...",
                    file=sys.stderr,
                )
                time.sleep(reconnect_delay_seconds)


def main() -> int:
    args = parse_args()
    return run_logger(port=args.port, baud=args.baud, output_path=args.output)


if __name__ == "__main__":
    sys.exit(main())
