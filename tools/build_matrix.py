#!/usr/bin/env python3
"""HPS compiler/standard matrix. Runs on the checked-out repository; no source rewrites."""
import argparse
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
STANDARDS = ("99", "11", "17", "23")
COMPILERS = {"gcc": "gcc", "clang": "clang"}
STRICT = (
    "-Wall -Wextra -Wpedantic -pedantic-errors -Wconversion "
    "-Wsign-conversion -Wshadow -Wstrict-prototypes "
    "-Wmissing-prototypes -Werror=implicit-function-declaration"
)

def run(cmd, *, log):
    print("+", " ".join(map(str, cmd)), flush=True)
    with log.open("a", encoding="utf-8") as output:
        output.write("+ " + " ".join(map(str, cmd)) + "\n")
        output.flush()
        result = subprocess.run(cmd, cwd=ROOT, stdout=output, stderr=subprocess.STDOUT)
    return result.returncode

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compilers", nargs="+", choices=tuple(COMPILERS), default=list(COMPILERS))
    parser.add_argument("--standards", nargs="+", choices=STANDARDS, default=list(STANDARDS))
    parser.add_argument("--strict", action="store_true", help="Enable additional diagnostics; failures count")
    args = parser.parse_args()
    failures = []
    for compiler in args.compilers:
        cc = shutil.which(COMPILERS[compiler])
        if cc is None:
            print("Missing compiler:", compiler, file=sys.stderr)
            return 2
        for std in args.standards:
            name = compiler + "-c" + std
            build = ROOT / "build" / "matrix" / name
            build.mkdir(parents=True, exist_ok=True)
            log = build / "matrix.log"
            log.write_text("", encoding="utf-8")
            flags = STRICT if args.strict else ""
            configure = ["cmake", "-S", str(ROOT), "-B", str(build),
                         "-DCMAKE_C_COMPILER=" + cc,
                         "-DHPS_C_STANDARD=" + std,
                         "-DCMAKE_C_EXTENSIONS=OFF",
                         "-DBUILD_TESTING=ON",
                         "-DCMAKE_C_FLAGS=" + flags]
            steps = [configure, ["cmake", "--build", str(build), "--parallel", "2"],
                     ["ctest", "--test-dir", str(build), "--output-on-failure"]]
            for step in steps:
                if run(step, log=log):
                    failures.append(name)
                    print("FAIL:", name, "see", log, flush=True)
                    break
            else:
                print("PASS:", name, flush=True)
    print("\nSummary:", "PASS" if not failures else "FAIL", "failed =", ", ".join(failures) or "none")
    return 1 if failures else 0

if __name__ == "__main__":
    raise SystemExit(main())
