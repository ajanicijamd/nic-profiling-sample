#include <hip/hip_runtime.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define HIP_ASSERT(x) (assert((x) == hipSuccess))

__global__ void vecAdd(
    double *a, double *b, double *c,
    int n
) {
    int id = blockIdx.x*blockDim.x +threadIdx.x;
    if (id < n) {
        c[id] = a[id] + b[id];
    }
}

int main() {
    const int LOOP_COUNT = 1000;
    int i;
    int n = 16;
    int blockSize;
    int gridSize;
    size_t bytes = n*sizeof(double);
    double *CPUArrayA, *CPUArrayB, *CPUArrayC;
    double *GPUArrayA, *GPUArrayB, *GPUArrayC;
    double *CPUVerifyArrayC;

    CPUArrayA = (double*)malloc(bytes);
    CPUArrayB = (double*)malloc(bytes);
    CPUArrayC = (double*)malloc(bytes);
    CPUVerifyArrayC = (double*)malloc(bytes);

    for (int i = 0; i < n; i++) {
        CPUArrayA[i] = i;
        CPUArrayB[i] = i;
    }

    HIP_ASSERT(hipMalloc(&GPUArrayA, bytes));
    HIP_ASSERT(hipMalloc(&GPUArrayB, bytes));
    HIP_ASSERT(hipMalloc(&GPUArrayC, bytes));

    HIP_ASSERT(hipMemcpy(GPUArrayA, CPUArrayA, bytes,
        hipMemcpyHostToDevice));
    HIP_ASSERT(hipMemcpy(GPUArrayB, CPUArrayB, bytes,
        hipMemcpyHostToDevice));

    for (int j = 0; j < LOOP_COUNT; j++) {

        blockSize = 1024;
        gridSize = (int)ceil((float)n/blockSize);
        vecAdd<<<gridSize, blockSize>>>(GPUArrayA, GPUArrayB, GPUArrayC, n);
        HIP_ASSERT(hipDeviceSynchronize());
        HIP_ASSERT(hipMemcpy(CPUArrayC, GPUArrayC, bytes,
            hipMemcpyDeviceToHost));

        // printf("Verifying the result...\n");
        timespec duration {
            .tv_sec = 0,
            .tv_nsec = 10000000
        };
        nanosleep(&duration, NULL);
        // sleep(1);
        printf("[%d]\n", j);

        for (i = 0; i < n; i++) {
            CPUVerifyArrayC[i] = CPUArrayA[i] + CPUArrayB[i];
        }

        for (i = 0; i < n; i++) {
            if (abs(CPUVerifyArrayC[i] - CPUArrayC[i]) > 1e-5) {
                printf("Error at position i = %d, Expected: %f, Found: %f\n",
                    i, CPUVerifyArrayC[i], CPUArrayC[i]);
                exit(1);
            }
        }

    }

    printf("Verified!\n");

    return 0;
}

// ...
