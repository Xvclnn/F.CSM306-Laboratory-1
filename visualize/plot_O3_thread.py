import csv
from collections import defaultdict
from statistics import mean
import matplotlib.pyplot as plt

def get_speedup_data(file_path, api_filter="threads"):
    """CSV-ээс өгөгдлийг уншиж, тухайн API-ийн хувьд speedup-ийг тооцоолно."""
    groups = defaultdict(list)
    try:
        with open(file_path, newline="") as f:
            reader = csv.DictReader(f)
            for row in reader:
                api = row["api"].strip().lower()
                if api != api_filter: continue
                
                t = int(row["threads"])
                dt = float(row["time"])
                groups[t].append(dt)
    except FileNotFoundError:
        print(f"Анхаар: {file_path} олдсонгүй.")
        return None, None

    sorted_threads = sorted(groups.keys())
    if not sorted_threads:
        return None, None
        
    avg_times = [mean(groups[t]) for t in sorted_threads]
    t1 = next((tm for t, tm in zip(sorted_threads, avg_times) if t == 1), None)
    
    if t1 is None:
        return None, None

    speedup = [t1 / tm for tm in avg_times]
    return sorted_threads, speedup

base_path = "../csv/"
files = {
    "Default": base_path + "result1.csv",
    "O1":      base_path + "result1_O1.csv",
    "O2":      base_path + "result1_O2.csv",
    "O3":      base_path + "result1_O3.csv"
}
styles = {
    "Default": {"color": "red",    "marker": "s", "ls": "-"},
    "O1":      {"color": "orange", "marker": "v", "ls": "--"},
    "O2":      {"color": "blue",   "marker": "^", "ls": "-."},
    "O3":      {"color": "green",  "marker": "o", "ls": "--"}
}

plt.figure(figsize=(12, 8))

for label, path in files.items():
    x, s = get_speedup_data(path, api_filter="threads")
    if x:
        st = styles[label]
        plt.plot(x, s, label=f"std::thread ({label})", 
                 color=st["color"], marker=st["marker"], 
                 linestyle=st["ls"], linewidth=2)
        plt.axhline(y=1.0, color=st["color"], linestyle=":", alpha=0.3)

plt.title("std::thread:")
plt.xlabel("Урсгалын тоо")
plt.ylabel("Хурдны өсөлт")
plt.ylim(0, 12) 
plt.grid(True, which='both', linestyle='--', alpha=0.5)
cpu_specs = "CPU: Intel i9-12900H (6P + 8E cores, 20 threads)"
plt.text(0.02, 0.98, cpu_specs, transform=plt.gca().transAxes, fontsize=10,
         verticalalignment='top', bbox=dict(boxstyle="round", facecolor='white', alpha=0.5))

plt.legend(loc="upper left", ncol=2)
plt.tight_layout()
plt.show()