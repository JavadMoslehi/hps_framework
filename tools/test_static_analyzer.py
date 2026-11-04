
#!/usr/bin/env python3

"""Verify GCC or Clang static analyzer diagnostics."""

import argparse
import subprocess
import sys
import tempfile
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--compiler",
        required=True,
    )

    parser.add_argument(
        "--analyzer",
        choices=("gcc", "clang"),
        required=True,
    )

    parser.add_argument(
        "--source",
        type=Path,
        required=True,
    )

    args = parser.parse_args()

    if not args.source.is_file():
        print("FAIL: Test source does not exist")
        return 1

    with tempfile.TemporaryDirectory(
            prefix="hps-static-"
    ) as temp_dir:

        if args.analyzer == "gcc":
            command = [
                args.compiler,
                "-std=c90",
                "-fanalyzer",
                "-Werror=analyzer-double-free",
                "-c",
                str(args.source.resolve()),
                "-o",
                str(Path(temp_dir) / "test.o"),
            ]

            expected = "-Werror=analyzer-double-free"

        else:
            command = [
                args.compiler,
                "--analyze",
                "-std=c90",
                "-Xanalyzer",
                "-analyzer-checker=unix.Malloc",
                "-Xanalyzer",
                "-analyzer-output=text",
                str(args.source.resolve()),
            ]

            expected = "Attempt to free released memory"

        try:
            result = subprocess.run(
                command,
                capture_output=True,
                text=True,
                errors="replace",
                timeout=60,
                check=False,
            )

        except (OSError, subprocess.TimeoutExpired) as error:
            print(f"FAIL: Analyzer execution error: {error}")
            return 1

    diagnostic = result.stdout + result.stderr

    if expected not in diagnostic:
        print("FAIL: Expected diagnostic was not found")
        print(diagnostic)
        return 1

    if args.analyzer == "gcc" and result.returncode == 0:
        print("FAIL: GCC did not reject the defect")
        return 1

    print(
        f"PASS: {args.analyzer} detected double free"
    )

    return 0


if __name__ == "__main__":
    sys.exit(main())
