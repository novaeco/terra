#!/usr/bin/env python3
"""
Quick ESP-IDF environment validation tailored for this project.

Checks:
- IDF_PATH is defined and points to an existing ESP-IDF tree.
- The argtable3 header that CMake copies during configuration exists (common Windows issue).
- Emits actionable remediation hints when the header is missing or unreadable.
"""
from __future__ import annotations

import os
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]


def _err(msg: str) -> None:
    print(f"[ERROR] {msg}")


def _info(msg: str) -> None:
    print(f"[INFO]  {msg}")


def _warn(msg: str) -> None:
    print(f"[WARN]  {msg}")


def validate_idf_path() -> Path:
    idf_path = os.environ.get("IDF_PATH", "").strip()
    if not idf_path:
        _err("IDF_PATH is not set. Activate the ESP-IDF environment or export.bat/export.sh first.")
        sys.exit(1)

    path = Path(idf_path).expanduser().resolve()
    if not path.exists():
        _err(f"IDF_PATH points to a non-existent directory: {path}")
        sys.exit(1)

    cmake_file = path / "CMakeLists.txt"
    if not cmake_file.is_file():
        _err(f"{path} does not look like an ESP-IDF tree (missing CMakeLists.txt)")
        sys.exit(1)

    _info(f"Using ESP-IDF at {path}")
    return path


def validate_argtable3_header(idf_path: Path) -> int:
    src_header = idf_path / "components" / "argtable3" / "argtable3" / "src" / "argtable3.h"
    include_dir = idf_path / "components" / "argtable3" / "include"
    exit_code = 0

    if src_header.is_file():
        _info(f"Found argtable3 header: {src_header}")
    else:
        exit_code = 2
        _err(
            "argtable3 header not found. CMake fails with 'file COPY cannot find ... argtable3.h'.\n"
            "Reinstall/refresh ESP-IDF and its submodules. Steps:\n"
            "  1) idf.py --version (verifies toolchain visibility)\n"
            "  2) git -C %IDF_PATH% submodule update --init --recursive\n"
            "  3) idf.py fullclean (inside the project) then idf.py set-target esp32s3\n"
            "  4) If still failing on Windows, run 'idf.py doctor' or reinstall via 'idf-env.exe install'"
        )
    if not include_dir.exists():
        exit_code = 3
        _warn(f"argtable3 include directory missing: {include_dir} (will be created by CMake if sources exist)")
    return exit_code


def main() -> None:
    idf_path = validate_idf_path()
    status = validate_argtable3_header(idf_path)

    if status == 0:
        _info("ESP-IDF environment looks sane for this project.")
    else:
        _warn("Environment checks reported issues. Resolve them before building.")
    sys.exit(status)


if __name__ == "__main__":
    main()
