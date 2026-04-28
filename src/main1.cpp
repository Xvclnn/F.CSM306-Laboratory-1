    #include <stdio.h>
    #include <string.h>
    #include <thread>
    #include <omp.h>
    #include <cstdlib>
    #include <ctime>
    #include <vector>
    #define SIZE 80000000
    using namespace std;

    void sumChunk(float* array, int start, int end, double* result, int index);
    void reduction(double* data, int i, int step) {
        data[i] += data[i + step];
    }
    struct benchmarkResult {
        double time_Serial;
        double time_OpenMP;
        double time_Threads;
        double result_OpenMP;
        double result_Threads;
    };

    benchmarkResult benchmark(int numThreads) {
        benchmarkResult result;

        float* array = (float*)malloc(SIZE*sizeof(float));
        #pragma omp parallel for simd num_threads(numThreads)
        for(int i = 0; i < SIZE; i++) {
            array[i] = 1.0f;
        }
        //serial 
        double sumSerial = 0.0f;
        double start_tsag = omp_get_wtime();
        for(int i = 0; i < SIZE; i++) {
            sumSerial += array[i];
        }
        double end_tsag = omp_get_wtime();
        result.time_Serial = end_tsag - start_tsag;

        double dummy1 = 0.0f;
        for(int i = 0; i < SIZE; i++) {
            dummy1 += array[i];
        }
        omp_set_dynamic(0);
        double sum1 = 0.0f;
        start_tsag = omp_get_wtime();    
        #pragma omp parallel for reduction(+:sum1) num_threads(numThreads)
        for(int i =0; i<SIZE; i++) {
            sum1 += array[i];
        }
        end_tsag = omp_get_wtime();
        result.time_OpenMP = end_tsag - start_tsag;
        result.result_OpenMP = sum1;

        dummy1 = 0.0f;
        for(int i = 0; i < SIZE; i++) {
            dummy1 += array[i];
        }

        vector<thread> threads(numThreads);
        vector<double> partialSums(numThreads, 0.0);
        int chunk = SIZE/numThreads;

        start_tsag = omp_get_wtime();
        for(int i=0; i<numThreads; i++) {
            int start = i*chunk;
            int end;
            if(i == numThreads -1) {
                end = SIZE;
            }
            else {
                end = start + chunk;
            }
            threads[i] = thread(sumChunk, array, start, end, partialSums.data(), i);
        }
        for (int t = 0; t < numThreads; t++) {
            threads[t].join();
        }
        
        for(int step = numThreads/2; step>0; step=step/2) {
            vector<thread>  reductionThread(step);
            for(int i =0; i<step; i++) {
                reductionThread[i] = thread(reduction, partialSums.data(), i, step);
            }
            for(int i = 0; i < step; i++) {
                reductionThread[i].join();
            }
        }
        double result_sum = partialSums[0];

        end_tsag = omp_get_wtime();
        result.time_Threads = end_tsag - start_tsag;
        result.result_Threads = result_sum;
        
        free(array);
        return result;
    }

    void sumChunk(float* array, int start, int end, double* partialSums, int index) {
        double sum = 0.0f;
        for(int i = start; i<end; i++) {
            sum += array[i];
        }
        partialSums[index] = sum;
    }

    int main() {
        FILE* fp =fopen("../csv/result1.csv", "w+");
        if(!fp) {
            printf("File нээхэд алдаа гарлаа !!!\n");
            return -1;
        }

        fseek(fp, 0, SEEK_END);
        if(ftell(fp) == 0) {
            fprintf(fp, "api,threads,time,benchmarkNumber\n");
        }

        for(int i= 1; i<=64; i=i*2) {
            for(int n =0; n<6; n++) {
                int numThreads = i;
                printf("Threads: %d\n", numThreads);
                benchmarkResult result = benchmark(numThreads);

                printf("Serial time: %f seconds\n", result.time_Serial);
                fprintf(fp, "serial,%d,%f\n", 1, result.time_Serial);
                
                printf("OpenMP time: %f seconds, result: %f\n", result.time_OpenMP, result.result_OpenMP);
                fprintf(fp, "openmp,%d,%f\n", numThreads, result.time_OpenMP);

                printf("Threads time: %f seconds, result: %f\n", result.time_Threads, result.result_Threads);
                fprintf(fp, "threads,%d,%f\n", numThreads, result.time_Threads);
            }   
        } 
        fclose(fp);
        return 0; 
    }