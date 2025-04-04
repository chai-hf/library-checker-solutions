#!/usr/bin/env python3
import os
import sys

def make_entry(title, path, indent=0):
    return "    " * indent + f"- [{title}]({path})"

def generate_summary(book_dir: str):
    benchmark_dir = os.path.join(book_dir, "benchmark")
    coverage_dir = os.path.join(book_dir, "coverage")
    summary_path = os.path.join(book_dir, "SUMMARY.md")

    summary = ["# Summary", "", "[Introduction](README.md)", "", "# Benchmark"]

    for root, _, files in os.walk(benchmark_dir):
        for f in sorted(files):
            if f.endswith(".md"):
                full_path = os.path.join(root, f)
                rel_path = os.path.relpath(full_path, book_dir).replace("\\", "/")

                relative_to_benchmark = os.path.relpath(full_path, benchmark_dir).replace("\\", "/")
                parts = relative_to_benchmark.split("/")

                program_dir = parts[-2] if len(parts) >= 2 else ""
                program_name = os.path.splitext(os.path.splitext(parts[-1])[0])[0]

                label = f"{program_dir}/{program_name}"
                summary.append(make_entry(label, rel_path))

    summary.append("")
    summary.append("# Coverage")

    for problem in sorted(os.listdir(coverage_dir)):
        problem_path = os.path.join(coverage_dir, problem)
        if not os.path.isdir(problem_path):
            continue

        for impl in sorted(os.listdir(problem_path)):
            impl_path = os.path.join(problem_path, impl)
            if os.path.isfile(impl_path) and impl_path.endswith(".md"):
                impl_name = os.path.splitext(os.path.basename(impl_path))[0]
                impl_dir = os.path.join(problem_path, impl_name)
                if not os.path.isdir(impl_dir):
                    rel_path = os.path.relpath(impl_path, book_dir).replace("\\", "/")
                    summary.append(make_entry(f"{problem}/{impl_name}", rel_path))

            elif os.path.isdir(impl_path):
                impl_name = os.path.basename(impl_path)
                impl_md = os.path.join(problem_path, f"{impl_name}.md")
                if os.path.exists(impl_md):
                    rel_path = os.path.relpath(impl_md, book_dir).replace("\\", "/")
                    summary.append(make_entry(f"{problem}/{impl_name}", rel_path))
                for f in sorted(os.listdir(impl_path)):
                    if f.endswith(".md"):
                        full_path = os.path.join(impl_path, f)
                        rel_path = os.path.relpath(full_path, book_dir).replace("\\", "/")
                        name = os.path.splitext(f)[0]
                        summary.append(make_entry(name, rel_path, indent=1))

    with open(summary_path, "w", encoding="utf-8") as fout:
        fout.write("\n".join(summary) + "\n")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: ./mdbook.py <book_directory>")
        sys.exit(1)
    generate_summary(sys.argv[1])
