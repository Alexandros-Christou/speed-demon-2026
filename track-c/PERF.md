# Track C performance record

All measurements below were made locally in Ubuntu/WSL2 on 30 October 2026. They are not results from the organiser's scoring machine, official board IDs, K values or competition scores.

## Approach and change

The first correct version stored a separate `unordered_set<string>` for every country. Repeated names therefore incurred many allocations and duplicate string copies across countries.

Version 2 interns each exact unescaped name once in a global `unordered_map<string, uint32_t>`. A country's dynamically sized bitset records membership. A new set bit increments the distinct counter. This preserves exact byte-sensitive equality while replacing repeated strings and hash nodes with bits. The map reserve is a capacity hint, not an assumed maximum number of names. Memory depends on the number of distinct names and active country bitsets; other distributions can change the tradeoff.

The parser, integer-cent sums and direct region lookup remain streaming. Raw record rows are not retained. Country groups are visited in encoded alphabetical order, with unmatched last.

## Actual local measurements

The updated official harness was used with two local CPU threads and the official full seed-1 data. Its correctness, timeout and sandbox gates passed in both versions.

| Version | Full container runs | Harness T (seconds) | Harness peak M (bytes) |
| --- | ---: | ---: | ---: |
| Per-country string sets | 1 | 7.388555 | 244,756,480 |
| Global interning and bitsets | 5 | 3.246994 | 19,492,864 |

The second batch passed the harness timing-spread check. The first version has only one container observation, so it provides no stability estimate. These are separate local batches and do not establish an official speedup. Version 2's pinned runtime image is 119,194,554 bytes.

An unsandboxed full differential check took 3.131522 seconds for version 2 and 18.827922 seconds for the Python reference in that invocation. This is a separate process-elapsed measurement, with possible cache and scheduling effects.

## Correctness evidence

Eleven exact comparisons cover tiny and sample seeds 1, 7 and 42; full seed 1; the official worked example; the worked example without its final terminator; header-only input; and a byte-preservation fixture. The last fixture includes UTF-8 names, trailing spaces, escaped quotes, quoted BOM/newline bytes, a record-boundary BOM, unmatched regions and negative cents.

The full output is 17,054 bytes with SHA256:

`d8088b1cd42ba1338b1345b107b55db7b96ef1f5066f3ff64fd26a8912a0f536`

Local evidence is in `research/speed-demon-2026/track-c-correctness-20261030.json`, `track-c-container-v1-20261030.json` and `track-c-container-v2-20261030.json`. The v1 source and correctness record are preserved under `research/speed-demon-2026/baselines`. Local harness IDs are not official board run IDs.

## Remaining checks

Alex must actually review and understand the implementation. Public OSI licence selection, repository publication, selected-repository board-app access and final declarations remain uncleared. Official board results and their citations must be added only after actual runs. Grammar, style and measurement consistency were reviewed internally with Codex; Grammarly was not used.

## Preparation status, 31 October 2026

The earlier pending-review notes describe the qualification stage. Alex has since replied "all g00d" to the review request and approved the MIT licence, required public release and selected-repository read-only board-app access. The MIT licence is now in the private repository. GitHub still requires Alex to complete Confirm access before app installation can finish. No official board run ID, official score or final submission receipt has been recorded. Local harness IDs remain local evidence only.
