#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define MAX_VAL 500 // Random values are [0, MAX_VAL]

// Matrix and vector sizes (5120: UHD TV)
#define N 5120

// Reference computation kernel (do not touch)
void bubble_sort_reference(double tab[N]) {
    size_t i, j;
    double temp;

    for (i = 0; i < N ; i++) {
        for (j = 0; j < N - i - 1; j++) {
            if (tab[j] > tab[j + 1]) {
                temp = tab[j + 1];
                tab[j + 1] = tab[j];
                tab[j] = temp;
            }
        }
    }
}

// Computation kernel (to parallelize)
void bubble_sort_kernel(double tab[N]) {
    size_t i, j;
    double temp;

    for (i = 0; i < N; i++) {
        #pragma omp parallel for private(temp)
        for (j = 0; j < N - 1; j += 2) {
            if (tab[j] > tab[j + 1]) {
                temp = tab[j + 1];
                tab[j + 1] = tab[j];
                tab[j] = temp;
            }
        }

        #pragma omp parallel for private(temp)
        for (j = 1; j < N - 1; j += 2) {
            if (tab[j] > tab[j + 1]) {
                temp = tab[j + 1];
                tab[j + 1] = tab[j];
                tab[j] = temp;
            }
        }
    }
}

int main() {
    double* a   = malloc(N * sizeof(double));
    double* ref = malloc(N * sizeof(double));
    double time_reference, time_kernel;

    // Initialization by random values
    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < N; i++)
        ref[i] = a[i] = (float)rand()/(float)(RAND_MAX/MAX_VAL);

    time_reference = omp_get_wtime();
    bubble_sort_reference(ref);
    time_reference = omp_get_wtime() - time_reference;
    printf("Reference time : %3.5lf s\n", time_reference);

    // for (size_t i = 0; i < N; ++i) {
    //     printf("%g ", ref[i]);
    // } printf("\n");

    time_kernel = omp_get_wtime();
    bubble_sort_kernel(a);
    time_kernel = omp_get_wtime() - time_kernel;
    printf("Kernel time    : %3.5lf s\n", time_kernel);

    // for (size_t i = 0; i < N; ++i) {
    //     printf("%g ", a[i]);
    // } printf("\n");

    // Check if the result differs from the reference
    for (size_t i = 0; i < N; i++) {
        if (ref[i] != a[i]) {
            // printf("Error on i = %ld, %g != %g\n", i, ref[i], a[i]);
            printf("Bad results :-(((\n");
            exit(1);
        }
    }
    printf("Speedup        : %g\n", (time_reference) / (time_kernel));
    printf("OK results :-)\n");

    free(a);
    free(ref);
    return 0;
}