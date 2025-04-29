#!/bin/bash
mount -t tmpfs -o size=32g none build
mount -t tmpfs -o size=32g none analysis
mount -t tmpfs -o size=32g none problems
echo 0 > /proc/sys/kernel/randomize_va_space
echo 0 > /proc/sys/kernel/perf_event_paranoid
echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo
for i in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor;do echo performance > $i;done
