#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define MAX_VAL 5     // Random values are [0, MAX_VAL]
#define N       1000000

/* ------------------------------------------------------------------------ *
 *      UTILITY FUNCTIONS NOT USEFUL FOR YOUR PURPOSE - DO NOT TOUCH        *
 * ------------------------------------------------------------------------ */

/**
 * print_sample function:
 * this function prints the first and last 'sample_length' elements of the
 * array of size 'size' pointed by 'tab'. If the array is not large enough,
 * it prints the complete array.
 * \param[in] tab           Pointer to the array to (partially) print.
 * \param[in] size          Size of the array.
 * \param[in] sample_length Number of first and last elements to print.
 */
void print_sample(double tab[], size_t size, size_t sample_length) {
    if (size <= 2 * sample_length) {
        for (size_t i = 0; i < size; i++)
            printf("%g ", tab[i]);
    } else {
        for (size_t i = 0; (i < size) && (i < sample_length); i++)
            printf("%g ", tab[i]);
        printf("... ");
        for (size_t i = size - sample_length; i < size; i++)
            printf("%g ", tab[i]);
    }
    printf("\n");
}

/**
 * quicksort_reference function:
 * this function sorts the range of elements of the array pointed by 'tab'
 * from element with index 'low' to element with index 'high'.
 * \param     tab  Pointer to the array to (partially) sort.
 * \param[in] low  Index of the first element to sort.
 * \param[in] high Index of the last  element to sort.
 */
void quicksort_reference(double tab[], int low, int high) {
    if (low < high) {
        // 1. Partition part
        // Take the last element as pivot, place it at its correct position
        // with smaller elements before it and greater elements after it.
        double pivot = tab[high];
        int pivot_location = low;
        double temp;
        for (size_t j = low; j < high; j++) {
            if (tab[j] < pivot) {
                temp = tab[pivot_location];
                tab[pivot_location] = tab[j];
                tab[j] = temp;
                pivot_location++;
            }
        }
        temp = tab[pivot_location];
        tab[pivot_location] = tab[high];
        tab[high] = temp;

        // 2. Recursive partition part on independent subarrays
        quicksort_reference(tab, low, pivot_location - 1);
        quicksort_reference(tab, pivot_location + 1, high);
    }
}

/**
 * quicksort_reference_driver function:
 * this function sorts the array of size 'size' pointed by 'tab'.
 * \param     tab  Pointer to the array to sort.
 * \param[in] size Size of the array.
 */
void quicksort_reference_driver(double* tab, size_t size) {
    quicksort_reference(tab, 0, size - 1);
}

/* ------------------------------------------------------------------------ *
 *       FUNCTIONS YOU HAVE TO PARALLELIZE USING OpenMP - DO TOUCH !        *
 * ------------------------------------------------------------------------ */

/**
 * quicksort_kernel function:
 * this function sorts the range of elements of the array pointed by 'tab'
 * from element with index 'low' to element with index 'high'.
 * \param     tab  Pointer to the array to (partially) sort.
 * \param[in] low  Index of the first element to sort.
 * \param[in] high Index of the last  element to sort.
 */
void quicksort_kernel(double tab[], int low, int high) {
    // Quick Sort-ing the parts of an array may be done in parallel.
    // For this purpose we may use tasks or sections, but using tasks
    // lets us to do parallelization only ones which leads to increase of performance.
    // Moreover, we may let other threads sort the array when the size is big to get
    // performance with parallelizm, but when the size of an array is small (1000 in this case),
    // we may let the current thread do the sorting process to save some time.
    if (low < high) {
        // 1. Partition part
        // Take the last element as pivot, place it at its correct position
        // with smaller elements before it and greater elements after it.
        double pivot = tab[high];
        int pivot_location = low;
        double temp;
        for (size_t j = low; j < high; j++) {
            if (tab[j] < pivot) {
                temp = tab[pivot_location];
                tab[pivot_location] = tab[j];
                tab[j] = temp;
                pivot_location++;
            }
        }
        temp = tab[pivot_location];
        tab[pivot_location] = tab[high];
        tab[high] = temp;

        char low_bool = 1, high_bool = 1;
        if (pivot_location - low < 1000) {
            quicksort_kernel(tab, low, pivot_location - 1);
            low_bool = 0;
        }

        if (high - pivot_location < 1000) {
            quicksort_kernel(tab, pivot_location + 1, high);
            high_bool = 0;
        }

        // 2. Recursive partition part on independent subarrays
        if (low_bool)
            #pragma omp task shared(tab)
            quicksort_kernel(tab, low, pivot_location - 1);
        if (high_bool)
            #pragma omp task shared(tab)
            quicksort_kernel(tab, pivot_location + 1, high);

        #pragma omp taskwait
    }
}

/**
 * quicksort_kernel_driver function:
 * this function sorts the array of size 'size' pointed by 'tab'.
 * \param     tab  Pointer to the array to sort.
 * \param[in] size Size of the array.
 */
void quicksort_kernel_driver(double* tab, size_t size) {
    #pragma omp parallel
    #pragma omp single
    quicksort_kernel(tab, 0, size - 1);
}

/* ------------------------------------------------------------------------ *
 *                      MAIN FUNCTION - DO NOT TOUCH                        *
 * ------------------------------------------------------------------------ */

int main() {
    double* a   = malloc(N * sizeof(double));
    double* ref = malloc(N * sizeof(double));
    double time_reference, time_kernel; 
        
    // Initialization by random values
    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < N; i++) {
        a[i] = (float)rand()/(float)(RAND_MAX/MAX_VAL);
        ref[i] = a[i];
    }

    time_reference = omp_get_wtime();
    quicksort_reference_driver(ref, N);
    time_reference = omp_get_wtime() - time_reference;
    printf("Reference time : %3.5lf s\n", time_reference);
    
    time_kernel = omp_get_wtime();
    quicksort_kernel_driver(a, N);
    time_kernel = omp_get_wtime() - time_kernel;
    printf("Kernel time    : %3.5lf s\n", time_kernel);

    print_sample(ref, N, 5);
    print_sample(a, N, 5);

    // Check if the result differs from the reference
    for (size_t i = 0; i < N; i++) {
        if (ref[i] != a[i]) {
            printf("Bad results :-(((\n");
            exit(1);
        }
    }
    printf("OK results :-)\n");
    
    free(a);
    free(ref);
    return 0;
}