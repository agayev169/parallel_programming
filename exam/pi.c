#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#define PI      "3.141592653589793238462"
#define ERROR   1.e-10   // Acceptable precision
#define MAX_VAL 5        // Random values are [0, MAX_VAL]
#define N       51200000 // Matrix and vector sizes (5120: UHD TV)

// Reference computation kernel (do not touch)
void pi_reference(size_t nb_steps, double* pi) {
    double term;
    double sum = 0.;
    double step = 1./(double)nb_steps;

    for (size_t i = 0; i < nb_steps; i++) {
        term = (i + 0.5) * step;
        sum += 4. / (1. + term * term);
    }

    *pi = step * sum;
}

// Computation kernel (to parallelize)
void pi_kernel(size_t nb_steps, double* pi) {
    // Addition process is independent, so we can parallelize it by reduction of the sum.
    // Another potential problem is that term must be private, so that there is no interruption 
    // between different processes.
    double term;
    double sum = 0.;
    double step = 1./(double)nb_steps;

    #pragma omp parallel for reduction(+:sum) private(term)
    for (size_t i = 0; i < nb_steps; i++) {
        term = (i + 0.5) * step;
        sum += 4. / (1. + term * term);
    }

    *pi = step * sum;
}

int main() {
    double pi, pi_ref;
    double time_reference, time_kernel; 
        
    time_reference = omp_get_wtime();
    pi_reference(N, &pi_ref);
    time_reference = omp_get_wtime() - time_reference;
    printf("Reference time: %3.5lf s\n", time_reference);
    
    time_kernel = omp_get_wtime();
    pi_kernel(N, &pi);
    time_kernel = omp_get_wtime() - time_kernel;
    printf("Kernel time   : %3.5lf s\n", time_kernel);

    printf("Pi (textbook) : %s\n", PI);
    printf("Pi reference  : %.22g\n", pi_ref);
    printf("Pi kernel     : %.22g\n", pi);

    // Check if the result differs from the reference
    if (fabs(pi_ref - pi) > ERROR) {
    //if (pi_ref != pi) {
        printf("Bad results :-(((\n");
        exit(1);
    }
    printf("OK results :-)\n");
    
    return 0;
}