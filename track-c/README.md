# Track C: exact CSV join and aggregation

This original C++20 implementation was written after the 30 October 2026 kickoff. It uses the standard library and C file I/O, with no analytics or database dependencies.

Build and run from this directory:

```sh
docker build --network=none -t speed-demon-c .
docker run --rm --network=none --read-only --cap-drop=ALL \
  -v "$PWD/data:/data:ro" -v "$PWD/out:/out" speed-demon-c
```

Create `out` first. Inputs are `records.csv` and `regions.csv`; the output is `aggregates.txt`. The Dockerfile pins both images and statically compiles the program. Those image digests must already be available for an offline build.

The streaming parser reuses field buffers. It unescapes doubled quotes, preserves embedded newlines and exact UTF-8 bytes, and strips a BOM only at a record boundary. CRLF and LF terminators, including a missing final terminator, are supported. The implementation relies on the published input bounds and three-uppercase-letter country codes.

A direct region lookup selects a country group. Countries are encoded in byte order, with unmatched records emitted last. Amounts use integer cents. Each exact name is interned once globally; each country stores a bitset of name IDs and an exact distinct count. Hash collisions are resolved by string equality. No probabilistic distinct-count approximation or precomputed result is used.

Eleven local differential cases passed against the official reference, including the full 3,320,000-record input. Its output matches the published SHA256. Five full local container runs also passed correctness, timeout and sandbox checks with stable timing spread. See `PERF.md` for measurements and limits.

Codex implemented and checked this code. Alex's understanding and review have not yet been recorded. No official board run, public repository, licence acceptance or final submission is claimed.

## With another 24 hours

With another 24 hours, I would measure name-interning hash-table overhead and the cost of sparse country bitsets on different valid name distributions. Any alternative must retain exact byte-sensitive counts and pass the same reference comparisons before replacing this version.
