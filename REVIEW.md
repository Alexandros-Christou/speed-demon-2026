# Alex's review before submission

Codex implemented and tested these tracks. This checklist is a guide for a real review, not evidence that Alex has completed one. No human understanding declaration has been made.

- **A, Ingest:** read `track-a/main.cpp`. Check how timestamps select hour groups, how the percentile rank is chosen, and how integer arithmetic prints error rates. The local full dataset and empty-input container checks passed.
- **C, Join:** read `track-c/main.cpp`. Check where BOMs are removed, how quotes and embedded newlines are preserved, why a global exact name ID plus per-country bitsets gives exact distinct counts, and why money uses integer cents. Eleven differential cases and five local full container runs passed.
- **D, Traverse:** read `track-d/main.cpp`. Check the CSR offsets, why different weak components prove no path but the same component does not prove a directed path, why the first valid target pop gives the shortest distance with non-negative weights, and how touched distances are reset after early returns. Actual qualification is recorded in `track-d/PERF.md`.

Flag anything you cannot explain, or any behaviour you want changed, before accepting the final declarations. Timings in these files are local observations. Official board runs, their IDs and any scoring claims must come from the organiser's actual board.

The code is in the private repository. Current approval and installation status are recorded below. No private research or another entrant's project is included.

## Review response on 31 October 2026

Alex replied “all g00d” to the request to review this guide and the track explanations. This records his actual response. He approved the MIT licence, required public release and selected-repository read-only app access; GitHub re-authentication is still required to finish installation.
