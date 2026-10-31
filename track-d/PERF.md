# Track D local qualification

This original solution was written during the permitted build window on 30 October 2026. The initial implementation uses a buffered ASCII parser, CSR adjacency arrays, weak-component and degree rejection, and forward Dijkstra search with a reusable binary heap. See `README.md` for the algorithm and correctness argument.

## Actual checks

Thirteen byte-exact comparisons passed against the official Python reference: the published worked example, an edgeless graph, no queries, three arbitrary all-pairs graphs, tiny seeds 1/7/42, sample seeds 1/7/42 and full seed 1. The arbitrary graphs were also checked against an independently implemented Floyd-Warshall oracle across all 961 ordered node pairs in each graph. A separate nine-query fixture passed both the reference and independently written answers for directed unreachability within one weak component and resets across early-return queries.

Both full input hashes match the published contract. The full result is 9,039 bytes, SHA256:

`a23dbdf0a3e49cc8901310a9ee1dc6a9a569a85297b6da75c2a7002cb2ed277e`

## Actual measurements

The updated official harness was run locally under Ubuntu/WSL2 and Docker 29.9.2, using two local CPU threads, offline digest-pinned builds and the full seed-1 dataset. Across 15 container runs in 3 batch(es), the final batch passed correctness, timeout and sandbox gates. Timing spread passed: True.

- Harness T: 16.907189 seconds.
- Harness peak M: 130,932,736 bytes.
- Runtime image: 119,169,014 bytes.

These are local observations, not the scoring hardware, board run IDs, K values, scores or a guaranteed speedup. All input parsing, graph construction, preprocessing and query answering are included. This is an initial baseline; no later optimisation or before/after improvement is claimed. The temporary raw edge list makes construction peak memory larger than the final adjacency structure.

Evidence: `research/speed-demon-2026/track-d-correctness-20261030.json`, `track-d-boundaries-20261030.json` and `track-d-container-v1-20261030.json`. Local harness IDs must not be cited as official board IDs.

## Remaining actions

Alex's actual understanding/review, public OSI licence, selected-repository board-app access, official runs and their PERF citations, final declarations and receipt remain outstanding. Codex performed the implementation, reference/oracle comparisons and internal grammar/style review. No human review or Grammarly use is claimed.

## Preparation status, 31 October 2026

The earlier pending-review notes describe the qualification stage. Alex has since replied "all g00d" to the review request and approved the MIT licence, required public release and selected-repository read-only board-app access. The MIT licence is now in the private repository. GitHub still requires Alex to complete Confirm access before app installation can finish. No official board run ID, official score or final submission receipt has been recorded. Local harness IDs remain local evidence only.
