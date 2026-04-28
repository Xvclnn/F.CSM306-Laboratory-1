#include <stdio.h>
#include <string.h>
#include <thread>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>
#include <chrono>

#define SIZE 80000000
using namespace std;

static inline double now_ms() {
    using clock = std::chrono::steady_clock;
    return std::chrono::duration<double, std::milli>(clock::now().time_since_epoch()).count();
}

void transformChunk(double* array, int start, int end) {
    for(int i = start; i < end; i++) {
        array[i] = sin(array[i]) * 0.5 + 0.25;
    }
}

struct benchmarkResult {
    double time_Serial_ms;
    double time_OpenMP_ms;
    double time_Threads_ms;
};

benchmarkResult benchmark(int numThreads) {
    benchmarkResult result;

    double* array = (double*)malloc((size_t)SIZE * sizeof(double));
    if(!array) {
        result.time_Serial_ms = -1.0;
        result.time_OpenMP_ms = -1.0;
        result.time_Threads_ms = -1.0;
        return result;
    }

    auto resetArray = [&]() {
        for(int i = 0; i < SIZE; i++) {
            array[i] = 1.0;
        }
    };

    resetArray();

    double warm = 0.0;
    for(int i = 0; i < SIZE; i++) warm += array[i];
    (void)warm;

    double start_tsag, end_tsag;

    resetArray();
    start_tsag = now_ms();
    for(int i = 0; i < SIZE; i++) {
        array[i] = sin(array[i]) * 0.5 + 0.25;
    }
    end_tsag = now_ms();
    result.time_Serial_ms = end_tsag - start_tsag;

    resetArray();
    omp_set_dynamic(0);
    start_tsag = now_ms();
    #pragma omp parallel for num_threads(numThreads)
    for(int i = 0; i < SIZE; i++) {
        array[i] = sin(array[i]) * 0.5 + 0.25;
    }
    end_tsag = now_ms();
    result.time_OpenMP_ms = end_tsag - start_tsag;

    resetArray();
    vector<thread> threads(numThreads);
    int chunk = SIZE / numThreads;

    start_tsag = now_ms();
    for(int i = 0; i < numThreads; i++) {
        int start = i * chunk;
        int end;
        if(i == numThreads - 1) end = SIZE;
        else end = start + chunk;
        threads[i] = thread(transformChunk, array, start, end);
    }
    for(int t = 0; t < numThreads; t++) {
        threads[t].join();
    }
    end_tsag = now_ms();
    result.time_Threads_ms = end_tsag - start_tsag;

    free(array);
    return result;
}

int main() {
    FILE* fp = fopen("../csv/result2.csv", "w+");
    if(!fp) {
        printf("File нээхэд алдаа гарсан!!\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0) {
        fprintf(fp, "api,threads,time\n");
    }

    const int warmupCount = 3;
    const int runCount = 10;

    for(int i = 1; i <= 64; i = i * 2) {
        int numThreads = i;
        printf("Threads: %d\n", numThreads);

        for(int w = 1; w <= warmupCount; w++) {
            benchmarkResult r = benchmark(numThreads);
            (void)r;
        }

        for(int run_i = 1; run_i <= runCount; run_i++) {
            benchmarkResult result = benchmark(numThreads);

            printf("Serial time: %f ms\n", result.time_Serial_ms);
            fprintf(fp, "serial,%d,%f,%d\n", 1, result.time_Serial_ms, i);

            printf("OpenMP time: %f ms\n", result.time_OpenMP_ms);
            fprintf(fp, "openmp,%d,%f,%d\n", numThreads, result.time_OpenMP_ms, i);

            printf("Threads time: %f ms\n", result.time_Threads_ms);
            fprintf(fp, "threads,%d,%f,%d\n", numThreads, result.time_Threads_ms, i);
        }
        fflush(fp);
    }

    fclose(fp);
    return 0;
}
