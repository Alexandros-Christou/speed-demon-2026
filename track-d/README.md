# Track D: exact directed shortest paths

This original C++20 implementation was written after the 30 October 2026 kickoff. It uses the standard library and C file I/O, with no graph-algorithm library or runtime dependencies beyond the standard toolchain.

Build and run from this directory, with the pinned images already available:

```sh
mkdir -p out
docker build --network=none -t speed-demon-d .
docker run --rm --network=none --read-only --cap-drop=ALL \
  -v "$PWD/data:/data:ro" -v "$PWD/out:/out" speed-demon-d
```

Inputs are `graph.txt` and `queries.txt`. Output is `distances.txt`. Every query produces one decimal distance or `-1`, in input order. All parsing, graph construction, preprocessing and searches occur during the timed process.

The program builds a compressed sparse row adjacency array. It preserves parallel edges, duplicates, zero weights and self-loops. Weak components are computed from the actual input using disjoint sets; this can reject pairs in different components, but never proves directed reachability within a component. A target with zero in-degree and a source with zero out-degree are also rejected, after handling the zero-length same-node path.

Remaining queries use exact forward Dijkstra search with a binary min-heap. A stale heap entry is skipped. The target is returned only when its current minimum distance is popped. Non-negative weights make that distance final. Distances use 64-bit unsigned integers, and valid finite answers fit in signed 64 bits under the published bounds. Only touched distances are cleared between searches. Early-return queries leave this list intact for the next search to reset.

The parser and integer ranges follow the published input contract. No coordinates, public-seed answers, fixed graph structures or assumed connectivity are encoded. The temporary edge list is released after the adjacency array is filled.

See `PERF.md` for actual local qualification and its limitations. Alex's understanding and review must be recorded separately from Codex implementation and testing. Public licensing, board-app access, official runs and final submission remain separate actions.

## With another 24 hours

With another 24 hours, I would profile graph parsing and per-query heap activity, then compare a two-pass CSR build or bidirectional search against this baseline. These are untested possibilities, not implemented improvements or measured speedups.
