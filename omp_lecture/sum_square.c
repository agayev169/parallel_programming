#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define ERROR   1.e-20 // Acceptable precision
#define MAX_VAL 5      // Random values are [0, MAX_VAL]

// Matrix and vector sizes (5120: UHD TV)
#define N 512000000

// Reference computation kernel (do not touch)
void sum_reference(double* psum, double* psum_square, double a[N]) {
    double sum        = 0.;
    double sum_square = 0.;

    for (size_t i = 0; i < N; i++) {
        sum += a[i];
        sum_square += a[i] * a[i];
    }

    *psum_square = sum_square;
    *psum        = sum;
}

// Computation kernel (to parallelize)
void sum_kernel(double* psum, double* psum_square, double a[N]) {
    double sum        = 0.;
    double sum_square = 0.;

    #pragma omp sections
    {
        #pragma omp section 
        #pragma omp parallel for num_threads(8) reduction(+:sum, sum_square)
        for (size_t i = 0; i < N; i++)
            sum += a[i];

        #pragma omp section
        #pragma omp parallel for num_threads(8) reduction(+:sum, sum_square)
        for (size_t i = 0; i < N; i++)
            sum_square += a[i] * a[i];
    }

    *psum_square = sum_square;
    *psum = sum;
}

int main() {
    double* a = malloc(N * sizeof(double));
    double ref, sum, ref_square, sum_square;
    double time_reference, time_kernel;

    // Initialization by random values
    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < N; i++) {
        a[i] = (float)rand()/(float)(RAND_MAX/MAX_VAL);
    }

    time_reference = omp_get_wtime();
    sum_reference(&ref, &ref_square, a);
    time_reference = omp_get_wtime() - time_reference;
    printf("Reference time : %3.5lf s\n", time_reference);

    time_kernel = omp_get_wtime();
    sum_kernel(&sum, &sum_square, a);
    time_kernel = omp_get_wtime() - time_kernel;
    printf("Kernel time    : %3.5lf s\n", time_kernel);

    // Check if the result differs from the reference
    if (abs(ref - sum) > ERROR || abs(ref_square - sum_square) > ERROR) {
        //if (ref != sum) {
        printf("Bad results :-(((\n");
        exit(1);
    }
    printf("OK results :-)\n");

    free(a);
    return 0;
}