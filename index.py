#!/usr/bin/env python3
import sys
import os
import shutil

def generate_links(title, cur_dir, root_dir, depth=2, strip_ext=None, strip_prefix=None):
    if not os.path.isdir(cur_dir):
        return ""
    lines = [f"<h{depth}>{title}</h{depth}>", "<ul>"]
    for dirpath, _, files in sorted(os.walk(cur_dir)):
        for file in sorted(files):
            if file.endswith('.md'):
                full_path = os.path.join(dirpath, file)
                rel_path = os.path.relpath(full_path, root_dir)
                label = rel_path
                if strip_ext and label.endswith(strip_ext):
                    label = label[: -len(strip_ext)]
                if strip_prefix and label.startswith(strip_prefix):
                    label = label[len(strip_prefix):].lstrip('/')
                lines.append(f"<li><a href='{rel_path}'>{label}</a></li>")
    lines.append("</ul>")
    return "\n".join(lines)

def collect_benchmarks(sol_dir, benchmark_dir):
    for dirpath, _, files in os.walk(sol_dir):
        for file in files:
            if file.endswith(".cpp.md"):
                src = os.path.join(dirpath, file)
                rel = os.path.relpath(src, sol_dir)
                dst = os.path.join(benchmark_dir, rel)
                os.makedirs(os.path.dirname(dst), exist_ok=True)
                shutil.copyfile(src, dst)

def main(root_dir):
    coverage_dir = os.path.join(root_dir, "coverage")
    cover_dir = os.path.join(coverage_dir, "cover")
    cpack_dir = os.path.join(coverage_dir, "cpack")
    benchmark_dir = os.path.join(coverage_dir, "benchmark")
    sol_dir = os.path.join(root_dir, "sol")
    collect_benchmarks(sol_dir, benchmark_dir)

    output = os.path.join(coverage_dir, "index.html")
    with open(output, "w", encoding="utf-8") as f:
        f.write("<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Coverage Index</title></head><body>\n")

        f.write("<iframe src='lcov/index.html' width='100%' height='800px' style='border:1px solid #ccc;'></iframe>\n")

        f.write("<div style='display: flex; justify-content: flex-start;'>\n")

        f.write("<div style='width: 20%; padding: 10px;'>\n")
        f.write(generate_links("benchmark", benchmark_dir, coverage_dir, strip_ext=".cpp.md", strip_prefix="benchmark"))
        f.write("</div>\n")

        f.write("<div style='width: 35%; padding: 10px;'>\n")
        f.write(generate_links("coverage", cover_dir, coverage_dir, strip_ext=".md", strip_prefix="cover"))
        f.write("</div>\n")

        f.write("<div style='width: 35%; padding: 10px;'>\n")
        f.write(generate_links("bundled-coverage", cpack_dir, coverage_dir, strip_ext=".md", strip_prefix="cpack"))
        f.write("</div>\n")

        f.write("</div>\n")
        f.write("</body></html>\n")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 index.py <solutions_root_dir>")
        sys.exit(1)
    main(sys.argv[1])
