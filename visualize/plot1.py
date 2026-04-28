import csv
from collections import defaultdict
from statistics import mean
import matplotlib.pyplot as plt

groups = defaultdict(list)
with open("../csv/result1.csv", newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        api = row["api"].strip().lower()
        t = int(row["threads"])
        dt = float(row["time"])
        groups[(api, t)].append(dt)

def avg_series(api_name: str):
    pts = [(t, mean(times)) for (api, t), times in groups.items() if api == api_name]
    pts.sort(key=lambda x: x[0])
    return [t for t, _ in pts], [v for _, v in pts]

x1, time_openmp = avg_series("openmp")
x2, time_threads = avg_series("threads")
x3, time_serial = avg_series("serial")

def to_speedup(x, times):
    t1 = None
    for th, tm in zip(x, times):
        if th == 1:
            t1 = tm
            break
    if t1 is None:
        raise ValueError("1 thread-ийн үр дүн олдсонгүй.")
    return [t1 / tm for tm in times]

speedup_openmp = to_speedup(x1, time_openmp)
speedup_threads = to_speedup(x2, time_threads)

x_all = sorted(set(x1) | set(x2) | set(x3))
serial_speedup = [1.0] * len(x_all)

plt.plot(x1, speedup_openmp, marker="o", label="OpenMP Speedup")
plt.plot(x2, speedup_threads, marker="s", label="std::thread Speedup")
plt.plot(x_all, serial_speedup, marker="^", label="Serial (baseline)")

plt.xlabel("thread-ийн тоо")
plt.ylabel("Хурд")
plt.title("OpenMP vs std::thread vs Serial")

cpu_specs = "CPU: Intel i9-12900H (6P + 8E cores, 20 threads)"
plt.text(
    0.02, 0.02, cpu_specs,
    transform=plt.gca().transAxes,
    fontsize=9,
    verticalalignment="bottom",
    bbox=dict(boxstyle="round", alpha=0.2)
)

plt.grid(True)
plt.legend()
plt.show()