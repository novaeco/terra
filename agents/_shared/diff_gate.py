#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
diff_gate.py — conservative diff scanner (anti-destruction)
Usage:
  python agents/_shared/diff_gate.py path/to/diff.patch

Exit codes:
  0 OK
  2 BLOCKER found
  1 usage / read error
"""

import sys, re, pathlib

BLOCKERS = [
    (re.compile(r"^deleted file mode\s+\d+", re.M), "File deletion detected"),
    (re.compile(r"^\-\-\- a/.*\n\+\+\+ /dev/null", re.M), "File removed in diff"),
    (re.compile(r"\bgit rm\b", re.I), "git rm command referenced"),
    (re.compile(r"^\-\s*(DROP\s+TABLE|DROP\s+COLUMN)\b", re.I | re.M), "Destructive SQL DROP detected"),
    (re.compile(r"^\-\s*lv_.*", re.M), "Potential broad removal (lv_*) — review"),
    (re.compile(r"(?i)\b(disable|off)\b.*\b(tls|https|jwt|auth|secure|encrypt)\b"), "Potential security downgrade"),
]

API_HINTS = [
    (re.compile(r"(?i)\b(route|endpoint|handler)\b.*\b(remove|delete)\b"), "Possible endpoint removal"),
    (re.compile(r"(?i)\b(unregister|deregister)\b.*\b(route|uri)\b"), "Possible route unregister"),
]

def main():
    if len(sys.argv) != 2:
        print("Usage: python agents/_shared/diff_gate.py <diff.patch>", file=sys.stderr)
        return 1
    p = pathlib.Path(sys.argv[1])
    if not p.exists():
        print(f"ERROR: diff file not found: {p}", file=sys.stderr)
        return 1
    txt = p.read_text(encoding="utf-8", errors="replace")

    findings = []

    for rx, msg in BLOCKERS:
        if rx.search(txt):
            findings.append(("BLOCKER", msg))

    # Heuristic: massive file changes
    file_headers = re.findall(r"^diff --git a/(.*?) b/(.*?)$", txt, flags=re.M)
    if len(file_headers) > 25:
        findings.append(("BLOCKER", f"Large patch touches {len(file_headers)} files (possible refactor)"))

    # Detect deleted/renamed files via diff metadata
    if re.search(r"^similarity index\s+\d+%$", txt, flags=re.M) and re.search(r"^rename from", txt, flags=re.M):
        findings.append(("BLOCKER", "File rename detected (potential refactor)"))

    # API hints
    for rx, msg in API_HINTS:
        if rx.search(txt):
            findings.append(("WARNING", msg))

    if findings:
        print("DIFF_GATE: findings")
        for level, msg in findings:
            print(f"- {level}: {msg}")
        # Conservative: any BLOCKER -> exit 2
        if any(level == "BLOCKER" for level, _ in findings):
            return 2
        return 0

    print("DIFF_GATE: OK (no blockers detected)")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
