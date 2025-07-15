# temporary

```sh
ulimit -s unlimited
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
echo 0 | sudo tee /proc/sys/kernel/perf_event_paranoid
sudo cpupower frequency-set -g performance
```

# persistent

```sh
echo '* soft stack 262144' | sudo tee -a /etc/security/limits.conf
echo 'kernel.randomize_va_space = 0' | sudo tee -a /etc/sysctl.conf
echo 'kernel.perf_event_paranoid = 0' | sudo tee -a /etc/sysctl.conf
```

# scaling

```sh
sudo vi /etc/systemd/system/set-cpu-governor.service
```

```ini
[Unit]
Description=Set CPU scaling governor to performance
After=multi-user.target

[Service]
Type=Oneshot
ExecStart=cpupower frequency-set -g performance
RemainAfterExit=true

[Install]
WantedBy=multi-user.target
```

```sh
sudo systemctl daemon-reload
sudo systemctl enable set-cpu-governor
```

# trubo

```sh
echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
echo 0 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
```
