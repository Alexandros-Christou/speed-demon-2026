# Track A: Ingest

This implementation reads every request once, groups its latency and error flag
by exact endpoint and UTC hour, then writes the specified ordered summary.

The timestamp grammar fixes both its width and its 72-hour range. The parser
therefore reads the month, day and hour at their known offsets. It does not parse
the unused minutes, seconds or fractional seconds. It skips the method, preserves
endpoint bytes and parses latency into an unsigned 32-bit integer.

An endpoint hash table owns each endpoint string once. Transparent lookup accepts
a view into the current input line, avoiding a temporary allocated string for
every request. Each endpoint holds 72 hour groups. All hour groups remain
available until the input ends, so there is no assumption about how late a shard
may arrive.

Each group stores its latencies, including duplicates. Sorting the vector gives
the three exact nearest-rank percentiles. Error counts and parts-per-million
arithmetic use 64-bit integers. Endpoints are sorted bytewise once; the output
loop visits hours first, then endpoints, and omits empty groups. Empty input
creates an empty output file.

## Build and run

```sh
docker build --network=none -t speed-demon-a track-a
docker run --rm --network=none --read-only \
  -v /absolute/input:/data:ro -v /absolute/output:/out:rw speed-demon-a
```

The output directory must exist and be writable. The only output is
`/out/summary.txt`. Optional positional directory arguments support local checks;
the container entry point uses the required `/data` and `/out` defaults.

Both Docker stages use the organiser's digest-pinned allowlist images. Compilation
is offline. There are no third-party runtime packages or network calls. The
program uses C++20, the standard library and Linux/POSIX file input.

## Further work

The first version sorts every latency vector. A measured selection algorithm may
reduce work for large groups, but should only replace sorting after repeatable
correctness and timing checks. Parallelism, custom allocators and SIMD have not
been implemented or benchmarked.
