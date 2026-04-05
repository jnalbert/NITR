# As-of Eligibility Evaluation

## Background

This repository evaluates whether a promotion rule is active based on its configured start and end timestamps.
Today it only supports evaluating eligibility at the current time.

Product now needs an as-of mode so replay, audit, and preview workflows can ask what the answer would have been at a specified Unix timestamp.
Existing current-time behavior must remain available.

## Required change

Update the eligibility service so callers can evaluate a promotion rule at a specified Unix timestamp instead of only using the current time.

## Behavior requirements

- Add support for evaluating a rule at a caller-provided Unix timestamp.
- Keep the existing current-time evaluation behavior available.
- A rule is active when the evaluation time falls within its configured active window.
- Use inclusive interval semantics: a rule is active when `start_timestamp <= evaluation_time <= end_timestamp`.
- Preserve existing behavior for callers that do not provide an explicit timestamp.
- Keep the change focused on this eligibility feature.

## Implementation constraints

- Do not modify files under `evaluator/022.as-of-eligibility-evaluation/`.
- You may add new files under `cases/022.as-of-eligibility-evaluation/src/`.
- Do not add third-party dependencies.
- Keep existing behavior unchanged unless required above.

## Build and test

The project must continue to compile, and all existing tests must still pass after the change.
