#!/usr/bin/env python3
import os, sys

def make_entry(title, path, indent=0):
    return "    " * indent + f"- [{title}]({path})"

def generate_summary(book_dir):
    benchmark_dir = os.path.join(book_dir, "benchmark")
    coverage_dir = os.path.join(book_dir, "coverage")
    summary_path = os.path.join(book_dir, "SUMMARY.md")

    summary = [
        "# Summary",
        "[Introduction](README.md)",
        "# Benchmark"
    ]

    for root, _, files in os.walk(benchmark_dir):
        for f in sorted(files):
            if f.endswith(".md"):
                p = os.path.relpath(os.path.join(root, f), book_dir).replace("\\", "/")
                summary.append(make_entry(os.path.splitext(f)[0], p))

    summary.append("# Coverage")

    for problem in sorted(os.listdir(coverage_dir)):
        prob_dir = os.path.join(coverage_dir, problem)
        prob_md = prob_dir + ".md"
        if os.path.isfile(prob_md):
            summary.append(make_entry(problem, os.path.relpath(prob_md, book_dir).replace("\\", "/")))
        if os.path.isdir(prob_dir):
            for f in sorted(os.listdir(prob_dir)):
                if f.endswith(".md"):
                    p = os.path.relpath(os.path.join(prob_dir, f), book_dir).replace("\\", "/")
                    summary.append(make_entry(os.path.splitext(f)[0], p, indent=1))

    with open(summary_path, "w", encoding="utf-8") as fout:
        fout.write("\n".join(summary) + "\n")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: mdbook.py <directory>")
        sys.exit(1)
    generate_summary(sys.argv[1])
