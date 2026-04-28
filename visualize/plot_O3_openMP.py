import csv
from collections import defaultdict
from statistics import mean
import matplotlib.pyplot as plt

def get_speedup_data(file_path):
    groups = defaultdict(list)
    try:
        with open(file_path, newline="") as f:
            reader = csv.DictReader(f)
            for row in reader:
                api = row["api"].strip().lower()
                if api != "openmp": continue
                t = int(row["threads"])
                dt = float(row["time"])
                groups[t].append(dt)
    except FileNotFoundError:
        return None, None
    sorted_threads = sorted(groups.keys())
    avg_times = [mean(groups[t]) for t in sorted_threads]
    t1 = None
    for t, tm in zip(sorted_threads, avg_times):
        if t == 1:
            t1 = tm
            break
    
    if t1 is None:
        return None, None

    speedup = [t1 / tm for tm in avg_times]
    return sorted_threads, speedup

path_default = "../csv/result1.csv"
path_o3 = "../csv/result1_O3.csv"
x_def, s_def = get_speedup_data(path_default)
x_o3, s_o3 = get_speedup_data(path_o3)

plt.figure(figsize=(12, 8))

if x_def:
    plt.plot(x_def, s_def, marker="s", color="red", label="openmp")
    plt.axhline(y=1.0, color="red", linestyle="--", alpha=0.6, label="Serial")

if x_o3:
    plt.plot(x_o3, s_o3, marker="o", color="green", linestyle="--", label="openmp (-O3)")
    plt.axhline(y=1.0, color="green", linestyle="-.", alpha=0.6, label="Serial (-O3)")

plt.title("Энгийн vs -O3 Optimization (openmp)")
plt.xlabel("Урсгалын тоо")
plt.ylabel("Хурдны өсөлт")
plt.ylim(0, 12) 
plt.grid(True, which='both', linestyle='--', alpha=0.5)
cpu_specs = "CPU: Intel i9-12900H (6P + 8E cores, 20 threads)"
plt.text(0.02, 0.95, cpu_specs, transform=plt.gca().transAxes, fontsize=10,
         verticalalignment='top', bbox=dict(boxstyle="round", facecolor='white', alpha=0.5))

plt.legend(loc="upper left")
plt.tight_layout()
plt.show()