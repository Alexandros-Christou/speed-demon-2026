# Track A performance log

## Baseline, 30 October 2026

The first entrant solution was written after the verified 15:00 UTC opening.
It replaces per-line calendar parsing and Python objects with fixed-offset
timestamp parsing, one owned string per endpoint and vectors of 32-bit latency
values. It keeps exact sorting as a clear correctness baseline.

Nine local comparisons matched the organiser's reference byte for byte:
tiny and sample scales at seeds 1, 7 and 42; full scale at seed 1; empty input;
and an additional adversarial input. The adversarial case covers all 500 allowed
HTTP statuses, all 72 hours, month rollover, a 125-byte endpoint, punctuation and
case-sensitive endpoint ordering, duplicate lines, unsigned latency limits and
percentile ranks around groups of 100 observations.

Generated seed-1 inputs matched the published input hashes at all three scales.
The full output was 10,113,024 bytes, with SHA-256
`495ca09682b9673ef836a506496177680bb4a3af54255174befddb1f83481cd3`.

On this local WSL machine, one unsandboxed full-scale process invocation took
0.667 seconds, compared with 39.419 seconds for one reference invocation. These
are single local elapsed measurements, including process launch, with different
cache states possible. They are not official timings, a competition speedup or
a live-board run. Raw records are retained separately.

## Official board and human review

The updated official harness also passed the full generated seed-1 dataset:
correctness, timeout and sandbox gates all passed on 15 local container runs.
The harness repeated the five-run batch twice because the first two batches had
too much timing spread. Its final batch met the spread threshold, reporting a
trimmed mean of 0.692902 seconds and a peak of 36,241,408 bytes for that batch.
The runtime image was 119,235,908 bytes. A separate empty-input container run
passed the same three gates and wrote the zero-byte summary required by the spec.

These results use this computer's WSL environment, two allowed local CPU threads
and the organiser's unmodified harness at commit
`39a24d1ec05d3f543d5138674f090fa71b63f25c`. They are local qualification results.
The run IDs in the saved JSON are local harness IDs, not live-board IDs.

No live-board run has been made. There are no board run IDs to cite. This log
must be extended with actual board evidence before it is represented as a final
optimisation history. No entrant human-understanding review has been confirmed.

Codex produced the implementation, documentation and local verification work.
No other model coding contribution is claimed.

## Preparation status, 31 October 2026

The earlier pending-review notes describe the qualification stage. Alex has since replied "all g00d" to the review request and approved the MIT licence, required public release and selected-repository read-only board-app access. The MIT licence is now in the private repository. GitHub still requires Alex to complete Confirm access before app installation can finish. No official board run ID, official score or final submission receipt has been recorded. Local harness IDs remain local evidence only.
