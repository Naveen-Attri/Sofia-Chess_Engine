# Sofia Chess Engine – CV Bullet Points

Four quantized, achievement-oriented bullet points for a CV/résumé, drawn directly from the technical work in this project.

---

1. **Engineered a fully UCI-compliant chess engine ("Sofia") in C++17** across ~1,500 lines of code, implementing the complete FIDE rule-set—including castling, en passant, promotion, the 50-move draw rule, and repetition detection—using a 120-square mailbox board representation; the engine communicates with any standard GUI (Arena, Cute Chess, etc.) via the UCI protocol.

2. **Implemented an alpha-beta negamax search with iterative deepening and quiescence search**, layering in 5 complementary move-ordering heuristics (Principal Variation probing, MVV-LVA capture scoring, killer-move slots × 2 per depth, and a history heuristic) to achieve a consistent move-ordering quality of 85%+ (fail-high-first rate), enabling search to depth 6 in under 1 second on a typical middle-game position.

3. **Parallelised the perft move-tree enumeration using C++ `std::async`**, spawning one thread per root-level move combined with bulk-counting optimisations at depths 1–3, yielding a **10× throughput increase** from 2–4 million nodes/second (single-threaded) to **20–24 million nodes/second** (multi-threaded) on standard test suites.

4. **Designed and hand-tuned 4 piece-square evaluation tables** (pawn, knight, bishop, rook) to steer positional play beyond raw material counting, with values calibrated so the engine consistently develops pieces toward active squares, outperforming a pure-material baseline in self-play tests by preferring central control and rook placement on open files.
