#!/usr/bin/env python3
import os
import sys
import shutil
import subprocess
import re
import math
from collections import defaultdict

def check_system_settings():
    # Check ASLR
    with open("/proc/sys/kernel/randomize_va_space", "r") as f:
        if f.read().strip() != "0":
            print("Error: ASLR is enabled. Please set /proc/sys/kernel/randomize_va_space to 0.")
            sys.exit(1)

    # Check if perf_event_paranoid
    with open("/proc/sys/kernel/perf_event_paranoid", "r") as f:
        value = int(f.read().strip())
        if value > 0:
            print(f"Error: /proc/sys/kernel/perf_event_paranoid is set as {value}. Please set it to 0 or lower.")
            sys.exit(1)

    # Check CPU governor
    for cpu in os.listdir("/sys/devices/system/cpu/"):
        if cpu.startswith("cpu") and cpu[3:].isdigit():
            gov_path = f"/sys/devices/system/cpu/{cpu}/cpufreq/scaling_governor"
            if os.path.exists(gov_path):
                with open(gov_path, "r") as f:
                    if f.read().strip() != "performance":
                        print(f"Error: CPU governor for {cpu} is not 'performance'.")
                        sys.exit(1)

    # Check Turbo Boost
    with open("/sys/devices/system/cpu/intel_pstate/no_turbo", "r") as f:
        if f.read().strip() != "1":
            print("Error: Turbo Boost is enabled. Please set /sys/devices/system/cpu/intel_pstate/no_turbo to 1.")
            sys.exit(1)

    # Check Hyperthreading
    # for cpu in os.listdir("/sys/devices/system/cpu/"):
    #     if cpu.startswith("cpu") and cpu[3:].isdigit():
    #         topo_path = f"/sys/devices/system/cpu/{cpu}/topology/thread_siblings_list"
    #         if os.path.exists(topo_path):
    #             with open(topo_path, "r") as f:
    #                 if len(f.read().strip().split(',')) != 1:
    #                     print(f"Error: Hyperthreading is enabled for {cpu}. Please disable it.")
    #                     sys.exit(1)

def extract_task_clock(perf_output):
    for line in perf_output.splitlines():
        if "task-clock" in line:
            return float(line.split()[0].replace(',', ''))  # Remove commas for consistency
    return float('inf')

def extract_memory_usage(time_output):
    match = re.search(r"(\d+)maxresident\)k", time_output)
    return int(match.group(1)) if match else 0

def parse_test_name(filename):
    match = re.match(r"(\D+)(\d+)?", filename)
    if match:
        return match.groups()
    return (filename, None)

def run_single_benchmark(exe, inputs, label):
    # Run time command to measure memory usage
    time_cmd = ["time", exe] + inputs
    time_result = subprocess.run(time_cmd, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, text=True)
    memory_usage = extract_memory_usage(time_result.stderr)

    # Run perf benchmark
    n = os.environ.get('N', '1')
    perf_cmd = ["perf", "stat", "-d", "-r", n, exe] + inputs
    perf_result = subprocess.run(perf_cmd, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, text=True)
    task_clock = extract_task_clock(perf_result.stderr)

    return (label, task_clock, memory_usage, "\n".join(perf_result.stderr.splitlines()[3:]), time_result.stderr)

def run_benchmark(exe, inputs, summary):
    check_system_settings()

    exe = "./" + exe
    results = []
    group_sizes = defaultdict(list)

    # Run ALL benchmark (combined input)
    results_all = run_single_benchmark(exe, inputs, "All")

    # Run benchmarks for each input file
    for input_file in inputs:
        test_name = os.path.basename(input_file).split('.')[0]
        result = run_single_benchmark(exe, [input_file], test_name)
        results.append(result)

        group_name, _ = parse_test_name(test_name)
        group_sizes[group_name].append(os.path.getsize(input_file))

    # Sort the groups and results
    sorted_groups = sorted(group_sizes.keys(), key=lambda g: (-sum(group_sizes[g]) / len(group_sizes[g]), g))
    results.sort(key=lambda x: (sorted_groups.index(parse_test_name(x[0])[0]), x[0]))

    # Statistics (exclude ALL)
    times = [r[1] for r in results]
    memories = [r[2] for r in results]

    summary_lines = [
        f"| Name                                               | Time (ms) | Memory (KB) |",
        f"|:---------------------------------------------------|----------:|------------:|",
        f"| Sum                                                | {sum(times):>9,.2f} | {sum(memories):>11,} |",
        f"| All                                                | {results_all[1]:>9,.2f} | {results_all[2]:>11,} |",
        f"| Max                                                | {max(times):>9,.2f} | {max(memories):>11,} |",
    ]

    for test_name, task_clock, memory_usage, _, _ in results:
        summary_lines.append(f"| {test_name:<50} | {task_clock:>9,.2f} | {memory_usage:>11,} |")

    os.makedirs(os.path.dirname(summary), exist_ok=True)
    with open(summary, 'w') as f:
        print("\n".join(summary_lines) + "\n\n")
        f.write("\n".join(summary_lines) + "\n\n")

        # Detailed outputs
        f.write(f"=== ALL ===\n```\n{results_all[4]}\n{results_all[3]}```\n")
        for test_name, _, _, perf_output, time_output in results:
            f.write(f"=== {test_name} ===\n```\n{time_output}\n{perf_output}```\n")

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: benchmark.py <exe> <inputs> <summary>")
        sys.exit(1)

    exe = sys.argv[1]
    inputs = sys.argv[2:-1]
    summary = sys.argv[-1]

    run_benchmark(exe, inputs, summary)
