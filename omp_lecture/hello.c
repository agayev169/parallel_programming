#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel num_threads(16)
    {
        #pragma omp parallel num_threads(24)
        {
            printf("Hello from %d,\n", omp_get_thread_num());
        }
    }
    printf("OpenMP\n");

    return 0;
}

// gcc -fopenmp hello_mess.c
// [OMP_NUM_THREADS=N] ./a.out