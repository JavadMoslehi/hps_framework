
#!/usr/bin/env python3

"""Strict sequential compiler/standard verification for HPS."""

import argparse
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

MATRIX = (
    "gcc-c89",
    "clang-c89",
    "gcc-c99",
    "clang-c99",
    "gcc-c11",
    "clang-c11",
    "gcc-c17",
    "clang-c17",
    "gcc-c23",
    "clang-c23",
)


CPP_MATRIX = (
    "gcc-c89",
    "clang-c89",
    "gcc-c89-cpp11",
    "clang-c89-cpp11",
    "gcc-c89-cpp14",
    "clang-c89-cpp14",
    "gcc-c89-cpp17",
    "clang-c89-cpp17",
    "gcc-c89-cpp20",
    "clang-c89-cpp20",
    "gcc-c89-cpp23",
    "clang-c89-cpp23",
)

def run_command(command, log_path):
    print("\n$ " + " ".join(str(arg) for arg in command), flush=True)

    with log_path.open("w", encoding="utf-8") as log_file:
        with subprocess.Popen(
                command,
                cwd=ROOT,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                errors="replace",
        ) as process:

            for line in process.stdout:
                print(line, end="", flush=True)
                log_file.write(line)

            return_code = process.wait()

    return return_code


def run_preset(name, report_dir):
    build_dir = ROOT / "build" / "matrix" / name

    steps = (
        ("configure", ["cmake", "--preset", name]),
        ("build", [
            "cmake", "--build", str(build_dir), "--verbose"
        ]),
        ("test", [
            "ctest",
            "--test-dir", str(build_dir),
            "--output-on-failure",
            "--no-tests=error",
        ]),
    )

    for step_name, command in steps:
        log_path = report_dir / f"{name}-{step_name}.log"

        result = run_command(command, log_path)

        if result != 0:
            print(
                f"\nFAILED: {name} / {step_name}"
                f"\nLog: {log_path}"
            )
            return False

    return True


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--c89-only",
        action="store_true",
        help="Run only GCC and Clang C89 gates",
    )
    parser.add_argument(
        "--cpp",
        action="store_true",
        help="Run the complete C/C++ interoperability matrix",
    )
    args = parser.parse_args()

    report_dir = ROOT / "build" / "matrix-reports"
    report_dir.mkdir(parents=True, exist_ok=True)

    if args.cpp and args.c89_only:
        parser.error("--cpp and --c89-only cannot be used together")

    if args.cpp:
        presets = CPP_MATRIX
    elif args.c89_only:
        presets = MATRIX[:2]
    else:
        presets = MATRIX

    for index, name in enumerate(presets, start=1):
        print(
            f"\n=== HPS MATRIX {index}/{len(presets)}: "
            f"{name} ===",
            flush=True,
        )

        if not run_preset(name, report_dir):
            return 1

        print(f"PASSED: {name}", flush=True)

    print(
        f"\nHPS MATRIX: {len(presets)}/{len(presets)} "
        "configurations passed.",
        flush=True,
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
