
#!/usr/bin/env python3

"""Verify that HPS rejects an incorrect C language standard."""

import subprocess
import sys
from pathlib import Path


def main():
    if len(sys.argv) != 3:
        print("Usage: test_language_negative.py COMPILER SOURCE")
        return 2

    compiler = sys.argv[1]
    source = Path(sys.argv[2])

    command = [
        compiler,
        "-std=c99",
        "-pedantic-errors",
        "-Werror",
        "-DHPS_EXPECTED_C_STANDARD=90",
        "-fsyntax-only",
        str(source),
    ]

    result = subprocess.run(
        command,
        capture_output=True,
        text=True,
        check=False,
    )

    expected_error = "HPS: Expected ISO C90"

    if result.returncode == 0:
        print("FAIL: Invalid language configuration was accepted")
        return 1

    if expected_error not in result.stderr:
        print("FAIL: Compiler failed for an unexpected reason")
        print(result.stderr)
        return 1

    print("PASS: Incorrect C standard was rejected")
    return 0


if __name__ == "__main__":
    sys.exit(main())
