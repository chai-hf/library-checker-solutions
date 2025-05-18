# Introduction

[Library Checker](https://judge.yosupo.jp) is an online judge for algorithmic problems, featuring hundreds of well-defined tasks such as polynomial exponentiation, double-ended priority queues, and more. All test case generators and tools are open-source on GitHub, allowing users to verify correctness, benchmark performance, and perform code coverage analysis locally. Contributions are welcome—whether it's proposing new problems or improving existing test data.

For benchmarking best practices, refer to [Benchmarking tips](https://llvm.org/docs/Benchmarking.html).

This repository contains my solutions to the problems, along with benchmark and coverage reports.

Benchmarking is done using `time` and `perf` to measure performance. See [unionfind](benchmark/unionfind-main.md) for an example. Since my solutions support multiple input files, I provide a special test case `All` that processes all queries in a batch. Ideally, its runtime is slightly less than the sum of individual test cases (`Sum`), and its memory usage slightly higher than the maximum (`Max`). This helps evaluate the efficiency of low-level optimizations.

Coverage testing is particularly useful for high-level optimization. For instance, in problems involving heavy-light decomposition, most queries tend to access prefixes of chains. Based on this insight, I apply segment tree optimizations tailored for prefix-heavy access patterns and validate them using coverage reports. In the slowest test case, comparing [main](coverage/vertex_set_path_composite-main/worst_for_path_decomposition_00.md) and [slow](coverage/vertex_set_path_composite-slow/worst_for_path_decomposition_00.md), we see that for segment merges (lines 19 and 20), the optimized version performs only 64% as many operations as the unoptimized one.
