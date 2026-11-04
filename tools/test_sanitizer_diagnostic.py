
#!/usr/bin/env python3

"""Verify that a sanitizer detects an intentional failure."""

import argparse
import os
import subprocess
import sys


EXPECTED_DIAGNOSTICS = {
    "asan": "AddressSanitizer: heap-use-after-free",
    "ubsan": "runtime error: signed integer overflow",
}


def main():
    parser = argparse.ArgumentParser(
        description="Validate an HPS sanitizer self-test"
    )

    parser.add_argument(
        "--sanitizer",
        choices=EXPECTED_DIAGNOSTICS.keys(),
        required=True,
    )

    parser.add_argument(
        "--executable",
        required=True,
    )

    args = parser.parse_args()

    environment = os.environ.copy()

    # Disable LeakSanitizer for this focused ASan self-test.
    environment["ASAN_OPTIONS"] = (
        "detect_leaks=0:halt_on_error=1:abort_on_error=0"
    )

    environment["UBSAN_OPTIONS"] = (
        "halt_on_error=1:print_stacktrace=1"
    )

    try:
        result = subprocess.run(
            [args.executable],
            capture_output=True,
            text=True,
            errors="replace",
            timeout=15,
            env=environment,
            check=False,
        )
    except subprocess.TimeoutExpired:
        print("FAIL: Sanitizer test timed out")
        return 1
    except OSError as error:
        print(f"FAIL: Cannot execute sanitizer test: {error}")
        return 1

    diagnostic = result.stdout + result.stderr
    expected = EXPECTED_DIAGNOSTICS[args.sanitizer]

    if result.returncode == 0:
        print("FAIL: Invalid operation was not detected")
        print(diagnostic)
        return 1

    if expected not in diagnostic:
        print("FAIL: Process failed for an unexpected reason")
        print(f"Exit code: {result.returncode}")
        print(diagnostic)
        return 1

    print(f"PASS: {args.sanitizer} detected the expected failure")
    return 0


if __name__ == "__main__":
    sys.exit(main())
