# 022. As-of Eligibility Evaluation

```yaml
case_id: 022-as-of-eligibility-evaluation
title: As-of eligibility evaluation
primary_dimension: D6 dependency_control
secondary_dimensions:
  - testability
language: C++
difficulty: easy-medium
loc: ~180-260
```

## Summary

This is a micro C++ maintainability case about adding an explicit as-of evaluation mode to a time-based eligibility service. The key structural question is whether current-time dependence remains embedded in core decision logic or is localized behind one clear boundary while preserving one shared eligibility path.

## Case Intent

Human reviewers should use this case to distinguish between:

- functionally correct but structurally weak solutions, which add an as-of entrypoint while keeping direct wall-clock reads or duplicated interval logic in core code
- maintainable solutions, which make current-time access a localized dependency and keep current-time and explicit-time evaluation converged on one shared decision path

This is primarily a dependency-control probe, not a general API design probe and not a pure testability probe. The visible product request is replay / audit / preview-at-time support. Testability improvement is a secondary signal, not the public framing.

The key taxonomy distinction is:

- this case is D6 because the main structural question is whether ambient wall-clock time remains a direct concrete dependency of core eligibility logic
- this case is not primarily D7 because the visible task is not "make time behavior easier to test"; deterministic testing is a downstream benefit of properly controlling the time dependency
- this case is not primarily D1 because the main failure is not callsite spread or API churn; the deeper issue is uncontrolled dependence on runtime environment state

## Problem Context

This service decides whether a promotion rule is active based on its configured start and end timestamps.
Today it only supports evaluating eligibility at the current time.
Product now needs an as-of mode so replay, audit, and preview flows can ask what the answer would have been at a specified Unix timestamp.
Existing current-time behavior must remain available.

## Required Repository Structure

Each case uses the following layout:

```text
- cases/022.as-of-eligibility-evaluation/
  - app/
    - main.cc
  - src/
  - TASK.md
  - CMakeLists.txt
- evaluator/022.as-of-eligibility-evaluation/
  - checks/
  - data/
  - tests/
- docs/022.as-of-eligibility-evaluation/
  - SPEC.md
```

All code must be in English.

## Given Code

The starter code should compile and pass its initial tests.

The initial repository should include:

- a small `PromotionRule`-like domain object under `cases/022.as-of-eligibility-evaluation/src/`
- an `EligibilityService`-like core API under `cases/022.as-of-eligibility-evaluation/src/`
- a small CLI entrypoint under `cases/022.as-of-eligibility-evaluation/app/main.cc`
- evaluator tests that already exercise baseline eligibility behavior
- evaluator structural checks that can detect direct system-time reads in core logic after the change

The intentional pressure point is that the starter implementation should read system time directly inside core decision logic. That should make the obvious wrong move attractive:

- keep current-time evaluation as the real logic
- add a second explicit-time path beside it
- duplicate the active-window check
- or scatter direct time reads across multiple functions

The case should stay narrow:

- one rule object
- one active-window decision
- one new explicit-time evaluation requirement
- no timezone handling
- no calendar parsing

## Expected Design Direction

Acceptable solutions may vary, but they should share one high-level property:
the rule-eligibility decision should have one clear authority for comparing a rule's active window against an evaluation time.

Good solution characteristics:

- current-time evaluation and explicit as-of evaluation converge on one decision path
- system time access is not scattered across core logic
- the new feature introduces one clear time boundary rather than duplicating active-window logic
- the existing current-time API can remain as a thin wrapper over the same underlying evaluation logic

The specification should not require one exact class name such as `Clock`, and it should not force one exact file split. The scored question is whether the dependency on current time remains controlled and localized.

## Evaluation

### Functional checks

The evaluator should verify at least:

- all existing tests still pass
- a rule is active when evaluated exactly at the start of the active window
- a rule is active when evaluated exactly at the end of the active window
- a rule is inactive before the start of the active window
- a rule is inactive after the end of the active window
- explicit as-of evaluation returns the correct answer for representative timestamps
- existing current-time evaluation still works

For this case, the active interval should be specified as inclusive on both ends:

```text
start_timestamp <= evaluation_time <= end_timestamp
```

The starter repository and evaluator should both use that rule explicitly.

### Structural checks

The evaluator should additionally verify:

- core decision logic does not read system time directly after the change
- current-time evaluation and explicit as-of evaluation converge on one underlying eligibility path rather than maintaining two independent implementations of the active-window comparison
- files under `evaluator/022.as-of-eligibility-evaluation/` are not modified

The structural rule about duplication should be interpreted narrowly:
- thin wrappers that forward into one shared implementation are acceptable
- maintaining two separate implementations of interval-comparison logic is not acceptable

### Maintainability checks

Signals of a strong solution:

- runtime-environment dependency on current time is localized
- current-time behavior is preserved without forcing direct wall-clock reads into core policy logic
- replay-style extension is supported without duplicating the active-window decision
- future time-based feature growth has one obvious place to plug in

Signals of a weak solution:

- business logic directly calls wall-clock APIs from core files
- current-time evaluation and explicit-time evaluation drift into separate implementations
- time access is spread across multiple core functions
- callers absorb current-time branching instead of keeping it behind one boundary

Implementation notes for the evaluator:

- direct use of system-time APIs such as `std::chrono::system_clock::now()` or equivalent wall-clock reads in core source files should be detectable through narrow static checks
- the evaluator should verify that current-time behavior is implemented through a thin wrapper or another converged path rather than a duplicated decision body
- the evaluator should prefer direct behavioral checks plus narrow ownership checks rather than forcing one exact abstraction name or file layout

## Failure Modes (Non-scoring)

- adds a second `IsEligibleAt(...)` path that duplicates the full active-window logic
- leaves the original `IsEligible(...)` path reading system time directly and keeps the new path structurally separate
- reads system time from multiple functions in the core module
- moves current-time branching into callers instead of localizing it behind one boundary
- expands unrelated APIs only to thread through timestamps unnecessarily
- introduces a global mutable time singleton or hidden global clock state to avoid localizing the dependency cleanly

## Maintainability Mapping

Primary Dimension:
  - D6 dependency_control
Measured Capability:
  - localize the dependency on current time
  - keep environmental time access outside the core eligibility decision path
Secondary Dimensions:
  - testability

## Distinctness From Nearby Cases

### Distinct From Case 009

Case `009.session-expiry-testability` is primarily about deterministic testing pressure around expiry behavior.

Case 022 is different:

- the visible request is a replay / audit / preview feature
- the pressure is not "please make this testable"
- the main hidden question is whether the core eligibility decision still depends directly on ambient wall-clock time

Testability improves in good solutions, but it is a secondary effect rather than the primary framing.

### Distinct From Cases 008 and 015

Cases `008.map-dip` and `015.pipeline-provider-decoupling` focus on broader collaborator or provider dependency structure.

Case 022 is narrower:

- one small domain decision
- one environment dependency: current time
- one explicit-time feature that should expose whether the dependency is controlled cleanly

The task does not revolve around provider construction, config wiring, or multi-provider orchestration.

### Global-State Rationale

Global mutable time state is disallowed because it is a common escape hatch for hiding time dependence without actually improving dependency control. Replacing direct wall-clock reads with a process-wide mutable singleton would still leave the case structurally weak.

## Allowed & Disallowed Summary

| Action                         | Allowed |
|--------------------------------|---------|
| Add new files                  | Yes     |
| Modify existing core logic     | Yes     |
| Modify existing tests          | No      |
| Add new dependencies           | No      |
| Modify public headers          | Yes     |
| Use global mutable state       | No      |
| Introduce new external IO      | No      |

## TASK.md Section

The agent-facing task should contain only the concise implementation request below, without describing the evaluation purpose or maintainability rationale.

### Task

Update the eligibility service so callers can evaluate a promotion rule at a specified Unix timestamp instead of only using the current time.
This new as-of mode is needed for replay and preview workflows, but the existing current-time behavior must remain available.

### Requirements

- Add support for evaluating a rule at a caller-provided Unix timestamp.
- Keep the existing current-time evaluation behavior available.
- A rule is active when the evaluation time falls within its configured active window.
- Use inclusive interval semantics: a rule is active when `start_timestamp <= evaluation_time <= end_timestamp`.
- Preserve existing behavior for callers that do not provide an explicit timestamp.
- Keep the change focused on this eligibility feature.

### Constraints

- Do not modify files under `evaluator/022.as-of-eligibility-evaluation/`.
- You may add new files under `cases/022.as-of-eligibility-evaluation/src/`.
- Do not add third-party dependencies.
- Keep existing behavior unchanged unless required above.
