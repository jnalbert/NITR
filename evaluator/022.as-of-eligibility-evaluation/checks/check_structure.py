#!/usr/bin/env python3

from pathlib import Path
import re
import sys


def read_text(path: Path) -> str:
    if not path.exists():
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def fail(message: str) -> int:
    print(message)
    return 1


def count_interval_checks(text: str) -> int:
    # This is intentionally heuristic rather than AST-precise. It tries to
    # catch the common maintainability failure mode of duplicating the active
    # window comparison body in the core implementation, but it may miss
    # semantically equivalent rewrites that use different local variable names
    # or extracted helpers.
    patterns = [
        r"start_timestamp\s*<=.*?&&.*?<=\s*.*end_timestamp",
        r"end_timestamp\s*>=.*?&&.*?>=\s*.*start_timestamp",
    ]
    total = 0
    for pattern in patterns:
        total += len(re.findall(pattern, text, flags=re.S))
    return total


def has_explicit_time_api(header_text: str, source_text: str) -> bool:
    patterns = [
        r"\bIsEligibleAt\s*\([^)]*PromotionRule[^)]*std::int64_t",
        r"\bIsEligible\s*\([^)]*PromotionRule[^)]*std::int64_t",
    ]
    combined = header_text + "\n" + source_text
    return any(re.search(pattern, combined, flags=re.S) for pattern in patterns)


def main() -> int:
    case_root = Path(sys.argv[1]) if len(sys.argv) > 1 else Path.cwd() / "cases/022.as-of-eligibility-evaluation"
    src_dir = case_root / "src"

    header_text = read_text(src_dir / "eligibility_service.h")
    source_text = read_text(src_dir / "eligibility_service.cc")

    if not has_explicit_time_api(header_text, source_text):
        return fail("EligibilityService must support explicit-time evaluation")

    forbidden_wall_clock_patterns = [
        r"std::chrono::system_clock::now\s*\(",
        r"std::chrono::steady_clock::now\s*\(",
        r"std::chrono::high_resolution_clock::now\s*\(",
        r"\bstd::time\s*\(",
        r"::time\s*\(",
    ]
    for pattern in forbidden_wall_clock_patterns:
        if re.search(pattern, source_text):
            return fail("core eligibility logic must not read wall-clock time directly")

    interval_check_count = count_interval_checks(source_text)
    if interval_check_count > 1:
        return fail("interval-comparison logic appears to be duplicated in eligibility_service.cc")

    print("case 022 structural checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
