
#!/usr/bin/env python3

"""
HPS Negative Static Assertion Verification.

The compiler must reject an intentionally false assertion
for the expected reason.

Supports:
    - ISO C90 using GCC or Clang
    - ISO C++98 using GCC or Clang

This script is a test utility, not a runtime dependency.
"""

import argparse
import subprocess
import sys
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(
        description="Verify negative HPS static assertions."
    )

    parser.add_argument(
        "--compiler",
        required=True,
        help="Absolute path to the compiler.",
    )

    parser.add_argument(
        "--language",
        choices=("c", "cpp"),
        required=True,
    )

    parser.add_argument(
        "--source",
        type=Path,
        required=True,
    )

    parser.add_argument(
        "--include",
        type=Path,
        required=True,
    )

    args = parser.parse_args()

    source = args.source.resolve()
    include_dir = args.include.resolve()

    if not source.is_file():
        print("FAIL: Test source does not exist.")
        return 1

    if not include_dir.is_dir():
        print("FAIL: Include directory does not exist.")
        return 1

    # Select a strict baseline language standard.
    if args.language == "c":
        language = "c"
        standard = "-std=c90"
    else:
        language = "c++"
        standard = "-std=c++98"

    command = [
        args.compiler,
        "-x",
        language,
        standard,
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-pedantic-errors",
        "-Werror",
        "-I",
        str(include_dir),
        "-fsyntax-only",
        str(source),
    ]

    print("HPS: Executing negative compilation test", flush=True)
    print("HPS: Language =", language, flush=True)

    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            errors="replace",
            timeout=30,
            check=False,
        )

    except (OSError, subprocess.TimeoutExpired) as error:
        print("FAIL: Compiler execution failed:", error)
        return 1

    diagnostic = result.stdout + result.stderr
    diagnostic_lower = diagnostic.lower()

    # The invalid source must fail compilation.
    if result.returncode == 0:
        print("FAIL: Invalid assertion was accepted.")
        return 1

    # Ensure the expected defect caused the failure.
    if "division by zero" not in diagnostic_lower:
        print("FAIL: Expected division-by-zero diagnostic missing.")
        print(diagnostic)
        return 1

    # Confirm the relevant assertion appears in diagnostics.
    if "intentional_failure" not in diagnostic:
        print("FAIL: Expected assertion identifier missing.")
        print(diagnostic)
        return 1

    print("PASS: Invalid static assertion was rejected.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
