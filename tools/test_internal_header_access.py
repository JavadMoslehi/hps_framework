
#!/usr/bin/env python3

"""
Verify HPS internal header access rules.

Cases:
  direct:
    Direct inclusion must be rejected for the expected reason.

  public:
    Inclusion through types.h must succeed, and the
    temporary internal access macro must not leak.

Supports ISO C90 and ISO C++98 test modes.
"""

import argparse
import subprocess
import sys
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--compiler", required=True)
    parser.add_argument("--include", type=Path, required=True)
    parser.add_argument(
        "--language",
        required=True,
        choices=("c", "cpp"),
    )
    parser.add_argument(
        "--mode",
        required=True,
        choices=("direct", "public"),
    )

    args = parser.parse_args()

    include_dir = args.include.resolve()

    if not include_dir.is_dir():
        print("FAIL: Include directory not found")
        return 1

    if args.mode == "direct":
        source = (
            "#include <hps/base/detail/stdint_fallback.h>\n"
        )
    else:
        source = (
            "#include <hps/base/types.h>\n"
            "#ifdef HPS_BASE_INSIDE\n"
            '#error "HPS: Internal access macro leaked"\n'
            "#endif\n"
            "int main(void) { return 0; }\n"
        )

    is_cpp = args.language == "cpp"

    command = [
        args.compiler,
        "-x",
        "c++" if is_cpp else "c",
        "-std=c++98" if is_cpp else "-std=c90",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-pedantic-errors",
        "-Werror",
        "-DHPS_LEGACY_NO_STDINT=1",
        "-I",
        str(include_dir),
        "-fsyntax-only",
        "-",
    ]

    try:
        result = subprocess.run(
            command,
            input=source,
            capture_output=True,
            text=True,
            errors="replace",
            timeout=30,
            check=False,
        )
    except (OSError, subprocess.TimeoutExpired) as error:
        print(f"FAIL: Compiler execution error: {error}")
        return 1

    diagnostic = result.stdout + result.stderr

    if args.mode == "direct":
        expected = (
            "HPS: Internal Base header cannot "
            "be included directly"
        )

        if result.returncode == 0:
            print("FAIL: Direct inclusion was accepted")
            return 1

        if expected not in diagnostic:
            print("FAIL: Rejected for an unexpected reason")
            print(diagnostic)
            return 1

    else:
        if result.returncode != 0:
            print("FAIL: Public inclusion was rejected")
            print(diagnostic)
            return 1

    print(
        f"PASS: {args.language} / {args.mode}"
    )

    return 0


if __name__ == "__main__":
    sys.exit(main())
