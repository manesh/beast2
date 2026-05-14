#!/usr/bin/env python3
"""Remove -2 / -3 export copies when they exactly match the original file.

Matching rule (strict):
  foo-2.md  -> compare with foo.md
  foo-3.md  -> compare with foo.md

No fuzzy or similar-name matching. Only suffixes -2 and -3 immediately before
the final extension are considered.
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

SUFFIX_RE = re.compile(r"^(.+)-([23])(\.[^.\\/]+)$")


def original_path(numbered: Path) -> Path | None:
    match = SUFFIX_RE.match(numbered.name)
    if match is None:
        return None
    base, _suffix, ext = match.groups()
    return numbered.with_name(f"{base}{ext}")


def files_equal(a: Path, b: Path) -> bool:
    return a.read_bytes() == b.read_bytes()


def iter_numbered_files(root: Path) -> list[Path]:
    numbered: list[Path] = []
    for path in root.rglob("*"):
        if not path.is_file():
            continue
        if SUFFIX_RE.match(path.name) is None:
            continue
        numbered.append(path)
    return sorted(numbered)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Delete -2/-3 files that are byte-identical to the original."
    )
    parser.add_argument(
        "directory",
        nargs="?",
        default="docs/chats",
        help="Root directory to scan (default: docs/chats)",
    )
    parser.add_argument(
        "--delete",
        action="store_true",
        help="Delete redundant files (default: dry run only)",
    )
    args = parser.parse_args()

    root = Path(args.directory).resolve()
    if not root.is_dir():
        print(f"error: not a directory: {root}", file=sys.stderr)
        return 1

    dry_run = not args.delete
    if dry_run:
        print("dry run (pass --delete to remove files)\n")

    deleted = 0
    skipped_no_original = 0
    skipped_diff = 0

    for numbered in iter_numbered_files(root):
        original = original_path(numbered)
        if original is None:
            continue

        rel_numbered = numbered.relative_to(root)
        if not original.is_file():
            print(f"skip (no original): {rel_numbered} -> {original.name}")
            skipped_no_original += 1
            continue

        if not files_equal(numbered, original):
            print(f"skip (different): {rel_numbered}")
            skipped_diff += 1
            continue

        print(f"delete: {rel_numbered}")
        if not dry_run:
            numbered.unlink()
        deleted += 1

    print()
    print(
        f"summary: {deleted} redundant, "
        f"{skipped_diff} different, "
        f"{skipped_no_original} missing original"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
