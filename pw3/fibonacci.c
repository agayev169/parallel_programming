#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

// -------------------------------------------------------
// Reference computation part (do not touch)

int fibor(int n) {
    if (n < 2)
        return n;

    return fibor(n-1) + fibor(n-2);
}

void fibonacci_reference(int n, int* fibo) {
    *fibo = fibor(n);
}

// -------------------------------------------------------
// Computation kernel (to parallelize)

int fibok(int n) {
    if (n < 2)
        return n;

    int f1, f2;

    if (n < 20)
        return fibok(n - 1) + fibok(n - 2);

    #pragma omp task shared(f1)
    f1 = fibok(n - 1); //, printf("Thread #%d\n", omp_get_thread_num());
    #pragma omp task shared(f2)
    f2 = fibok(n - 2); //, printf("Thread #%d\n", omp_get_thread_num());
    #pragma omp taskwait

    return f1 + f2;
}

void fibonacci_kernel(int n, int* fibo) {
    #pragma omp parallel
    #pragma omp single
    *fibo = fibok(n);
}

// -------------------------------------------------------

int main(int argc, char* argv[]) {
    double time_reference, time_kernel;
    int n, fibo_ref, fibo_ker;
    
    if (argc != 2) { 
        fprintf(stderr, "usage: %s number\n", argv[0]);
        exit(1);
    }
    n = atoi(argv[1]);

    time_reference = omp_get_wtime();
    fibonacci_reference(n, &fibo_ref);
    time_reference = omp_get_wtime() - time_reference;
    printf("Reference time : %3.5lf s\n", time_reference);
    
    time_kernel = omp_get_wtime();
    fibonacci_kernel(n, &fibo_ker);
    time_kernel = omp_get_wtime() - time_kernel;
    printf("Kernel time    : %3.5lf s\n", time_kernel);

    // Check if the result differs from the reference
    if (fibo_ref != fibo_ker) {
        printf("Bad results :-(((\n");
        printf("Reference: fibo(%d) = %d\n", n, fibo_ref);
        printf("Kernel:    fibo(%d) = %d\n", n, fibo_ker);
        exit(1);
    }
    printf("fibo(%d) = %d\n", n, fibo_ref);
    printf("Speedup        : %g\n", (time_reference) / (time_kernel));
    printf("OK results :-)\n");

    return 0;
}