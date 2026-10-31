# Speed Demon 2026: Alex C. Christou

Three original C++20 implementations, begun after the verified 30 October 2026 15:00 UTC kickoff:

| Track | Task | Implementation |
| --- | --- | --- |
| A | Log aggregation | Fixed-format parsing, hour/endpoint groups and exact nearest-rank percentiles |
| C | CSV join | Byte-preserving streaming CSV, integer cents, exact name interning and per-country bitsets |
| D | Directed shortest paths | CSR adjacency, necessary reachability rejections and exact 64-bit Dijkstra searches |

Each track directory contains source, a digest-pinned Dockerfile, `.speed-demon.toml`, `README.md` and `PERF.md`. No graph, dataframe or database algorithm libraries are used. Build and run instructions are in the individual READMEs. All preprocessing happens during execution; no dataset or precomputed answer is embedded.

Local checks passed against the official reference and published full seed-1 output hashes. A passed nine differential cases, C eleven, and D thirteen plus a separate directed/reset fixture. D's arbitrary small graphs were also checked against an independent Floyd-Warshall oracle.

| Track | Local harness T, seconds | Local harness peak M, bytes |
| --- | ---: | ---: |
| A | 0.692902 | 36,241,408 |
| C | 3.246994 | 19,492,864 |
| D | 16.907189 | 130,932,736 |

These are local Ubuntu/WSL2 observations, not official board results, K values or scores. Each track's final full-data batch passed correctness, timeout, sandbox and timing-spread checks. Retried batches and actual methodology are described in the PERF files. Official run citations remain outstanding.

Codex implemented and tested the code. Alex replied ‘all g00d’ to the review request on 31 October 2026. He approved the MIT licence, public release for code freeze and selected-repository read-only board-app access. Installation is awaiting GitHub re-authentication; final declarations remain outstanding. The code is MIT-licensed. This private review packet is not a submitted entry.

## Submission preparation

The private repository is `Alexandros-Christou/speed-demon-2026`; the required branch is `speed-demon`. Alex approved the MIT licence and public release for code freeze on 31 October. The repository remains private until that release. A private repository is not an official entry or board result.
