# CSM306 Laboratory 1: Parallel Programming Benchmarking

A comprehensive empirical study comparing the performance of serial, OpenMP, and C++ thread-based implementations for compute-intensive operations across varying thread counts and compiler optimization levels.

## Overview

This laboratory assignment explores parallelization strategies for compute-bound and memory-intensive operations. The study benchmarks three programming paradigms (sequential, OpenMP directives, and native C++ threads) across multiple system configurations to analyze scalability, overhead, and optimization effectiveness.

**Course:** CSM306 - Parallel Programming  
**Laboratory:** 1 - Parallelization Techniques & Performance Analysis

---

## Project Structure

```
F.CSM306-Laboratory-1/
├── src/
│   ├── main1.cpp          # Experiment 1: Array sum reduction benchmark
│   └── main2.cpp          # Experiment 2: Computational workload (transcendental functions)
├── csv/
│   ├── result1*.csv       # Benchmark results for Experiment 1
│   ├── result2*.csv       # Benchmark results for Experiment 2
│   └── result*.csv        # Aggregated results across all configurations
├── visualize/
│   ├── plot1.py           # Visualization for Experiment 1 results
│   ├── plot2.py           # Visualization for Experiment 2 results
│   ├── plot_O3_openMP.py  # O3 optimization level analysis (OpenMP)
│   └── plot_O3_thread.py  # O3 optimization level analysis (Threads)
├── images/                # Generated plots and analysis figures
└── README.md              # This file
```

---

## Experiments

### Experiment 1: Array Reduction (`main1.cpp`)

**Objective:** Benchmark array summation with varying thread counts and parallelization approaches.

**Implementation Details:**

- **Array Size:** 80,000,000 elements (float)
- **Operations:** Serial summation vs. OpenMP reduction vs. C++ thread-based reduction
- **Metrics:** Execution time, speedup, parallel efficiency

**Parallelization Methods:**

1. **Serial:** Standard sequential loop accumulation
2. **OpenMP:** `#pragma omp parallel for reduction(+:sum)` directive
3. **C++ Threads:** Manual thread pool with workload distribution

---

### Experiment 2: Computational Workload (`main2.cpp`)

**Objective:** Evaluate parallelization effectiveness on compute-intensive operations.

**Implementation Details:**

- **Array Size:** 80,000,000 elements (double)
- **Operation:** Element-wise sine transformation: `array[i] = sin(array[i]) * 0.5 + 0.25`
- **Metrics:** Execution time (milliseconds), scalability analysis

**Parallelization Methods:**

1. **Serial:** Direct loop evaluation
2. **OpenMP:** Parallel for loop with thread binding
3. **C++ Threads:** Chunk-based task distribution

---

## Building

### Prerequisites

- **Compiler:** GCC/G++ 7.0+ with OpenMP support
- **Build Tool:** GNU Make or manual compilation
- **Python:** 3.6+ (for visualization)
- **Python Libraries:** matplotlib, numpy, pandas

### Compilation Options

The project supports multiple compiler optimization levels and configurations:

```bash
# Standard build (no optimization)
g++ -std=c++11 -fopenmp -o main1 src/main1.cpp

# With O1 optimization
g++ -std=c++11 -fopenmp -O1 -o main1 src/main1.cpp

# With O3 optimization (recommended)
g++ -std=c++11 -fopenmp -O3 -march=native -o main1 src/main1.cpp

# With special configurations
g++ -std=c++11 -fopenmp -O3 -floop-unroll-and-jam -o main1_unroll src/main1.cpp
```

---

## Running Benchmarks

### Execute Individual Experiments

```bash
# Run Experiment 1 with thread counts 1-16
./main1 2>/dev/null | tee result1.csv

# Run Experiment 2 with thread counts 1-16
./main2 2>/dev/null | tee result2.csv
```

**Output Format (CSV):**

```csv
api,threads,time
serial,1,xxxx.xx
openmp,1,xxxx.xx
threads,1,xxxx.xx
...
```

### Batch Execution

Compile and run experiments with all optimization levels:

```bash
# Compile with O1
g++ -std=c++11 -fopenmp -O1 -o main1_O1 src/main1.cpp && ./main1_O1 > csv/result1_O1.csv

# Compile with O3
g++ -std=c++11 -fopenmp -O3 -march=native -o main1_O3 src/main1.cpp && ./main1_O3 > csv/result1_O3.csv
```

---

## Visualization & Analysis

### Generate Performance Plots

```bash
cd visualize

# Experiment 1 analysis
python3 plot1.py

# Experiment 2 analysis
python3 plot2.py

# Optimization level comparison (O3)
python3 plot_O3_openMP.py
python3 plot_O3_thread.py
```

**Output:** PNG plots saved to `../images/`

### Key Metrics Generated

- **Execution Time vs. Thread Count:** Shows scaling behavior
- **Speedup Curve:** Parallel speedup relative to serial baseline
- **Efficiency Analysis:** Parallel efficiency (Speedup / Thread Count)
- **Optimization Impact:** Comparison across compiler flags

---

## Results

### Data Files

| File                   | Experiment | Description                       |
| ---------------------- | ---------- | --------------------------------- |
| `result1.csv`          | Exp 1      | Array sum reduction - baseline    |
| `result1_O1.csv`       | Exp 1      | O1 optimization                   |
| `result1_O3.csv`       | Exp 1      | O3 optimization                   |
| `result1_overload.csv` | Exp 1      | Overload configuration            |
| `result1_oversubs.csv` | Exp 1      | Oversubscription test             |
| `result2.csv`          | Exp 2      | Computational workload - baseline |
| `result2_O3.csv`       | Exp 2      | O3 optimization                   |

### Expected Outcomes

1. **Experiment 1 (Reduction):**
   - Limited speedup due to memory-bound nature of summation
   - OpenMP typically outperforms manual threading due to reduction optimization
   - Diminishing returns beyond 4-8 threads on modern systems

2. **Experiment 2 (Compute):**
   - Significant speedup potential (near-linear scaling with 2-4 cores)
   - Compute-intensive operations scale better than memory-bound workloads
   - OpenMP and threads show comparable performance at O3 optimization

---

## Technical Notes

### Thread Synchronization

- **OpenMP:** Automatic synchronization via compiler directives
- **C++ Threads:** Manual synchronization using thread joining and atomic operations

### Memory Allocation

- Heap allocation (malloc/new) used for large arrays to exceed L3 cache
- Warm-up iterations performed to eliminate initialization overhead

### Timing Considerations

- Multiple runs aggregated to account for system variance
- High-resolution timers used (omp_get_wtime, std::chrono::steady_clock)
- Dummy operations included to prevent compiler optimizations of benchmarked code

---

## Dependencies

- **C++ Standard Library:** <thread>, <chrono>
- **OpenMP:** libomp (included with most modern GCC installations)
- **Python Visualization Stack:** matplotlib, csv, statistics modules

## References

- OpenMP API Specification (www.openmp.org)
- C++ Standard Library Documentation
- Performance Counter Analysis Techniques
- Amdahl's Law and Gustafson's Law
