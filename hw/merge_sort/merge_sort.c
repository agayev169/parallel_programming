#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int is_proc_nb_valid();
void quicksort(int *arr, int arr_size);
void _quicksort(int *arr, int low, int high);
int partition(int *arr, int low, int hig);
void swap(int *a, int *b);
int *mergesort(int *arr, size_t arr_size);

int arr_max = 1000000;
int main(int argc, char *argv[]) {
    srand(time(NULL));

    size_t arr_size = 100000;

    int arr[arr_size];
    for (size_t i = 0; i < arr_size; ++i) {
        arr[i] = rand() % (arr_max + 1);
    }

    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // If there are not 2^n processes or arr_size is not divisible by number of processes
    int proc_n;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

    if (!is_proc_nb_valid()) {
        if (rank == 0)
            fprintf(stderr, "Number of processes must be a power of 2\n");
        MPI_Finalize();
        exit(1);
    }

    if (arr_size % proc_n != 0) {
        if (rank == 0)
            fprintf(stderr, "Array size must be divisible by the number of processes\n");
        MPI_Finalize();
        exit(1);
    }

    // Quick Sort
    size_t proc_arr_size = arr_size / proc_n;

    // printf("process #%d sorts elements from %lu to %lu\n", rank, proc_arr_size * rank, proc_arr_size * (rank + 1));
    quicksort(arr + proc_arr_size * rank, proc_arr_size - 1);

    // printf("process #%d results: ", rank);
    // for (size_t i = rank * proc_arr_size; i < (rank + 1) * proc_arr_size; ++i) {
    //     printf("%d ", arr[i]);
    // }
    // printf("\n");

    int *new_arr = mergesort(arr, arr_size);

    // if (rank == 0) {
    //     for (size_t i = 0; i < arr_size; ++i) {
    //         printf("%d ", new_arr[i]);
    //     }
    //     printf("\n");
    // }

    free(new_arr);

    MPI_Finalize();
}

int is_proc_nb_valid() {
    int proc_n;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

    while (proc_n > 1) {
        if (proc_n % 2 != 0) return 0;
        proc_n /= 2;
    }
    return 1;
}

void quicksort(int *arr, int arr_size) {
    _quicksort(arr, 0, arr_size);
}

void _quicksort(int *arr, int low, int high) {
    // https://www.geeksforgeeks.org/quick-sort/
    if (low < high) {
        /* pi is partitioning index, arr[pi] is now
           at right place */
        int pi = partition(arr, low, high);

        _quicksort(arr, low, pi - 1);  // Before pi
        _quicksort(arr, pi + 1, high); // After pi
    }
}

int partition(int *arr, int low, int high) {
    // https://www.geeksforgeeks.org/quick-sort/
    // pivot (Element to be placed at right position)
    int pivot = arr[high];    // pivot 
    int i = (low - 1);  // Index of smaller element 
  
    for (int j = low; j <= high - 1; j++) { 
        // If current element is smaller than the pivot 
        if (arr[j] < pivot) { 
            i++;    // increment index of smaller element 
            swap(&arr[i], &arr[j]); 
        } 
    } 
    swap(&arr[i + 1], &arr[high]); 
    return (i + 1); 
}

void swap(int *a, int *b) { 
    int t = *a; 
    *a = *b; 
    *b = t; 
} 

int *mergesort(int *arr, size_t arr_size) {
    int proc_n;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // int new_arr[arr_size];

    int *new_arr = malloc(arr_size * sizeof(int));

    // printf("process #%d enters mergesort()\n", rank);

    // if (rank == 0) {
    //     printf("arr: ");
    //     for (size_t i = 0; i < arr_size; ++i) {
    //         printf("%d ", arr[i]);
    //     }
    //     printf("\n");
    // }

    if (rank == 0) { // Merging
        size_t arr_size_proc = arr_size / proc_n;
        size_t indexes[proc_n]; // current index of each process
        size_t index = 0;       // current index
        for (size_t i = 0; i < (size_t) proc_n; ++i) {
            indexes[i] = 0;
        }

        int val;                   // current val
        int min_val = arr_max + 1; // min_val;
        int min_val_proc = 0;      // process that sent min_val
        char received[proc_n];
        received[0] = 0;
        char break_ = 1;

        int zero = 0;
        int one  = 1;

        while (1) {
            // Receive nums
            for (size_t i = 1; i < (size_t) proc_n; ++i) {
                if (indexes[0] < arr_size_proc) {
                    min_val = arr[indexes[0]];
                    min_val_proc = 0;
                    break_ = 0;
                }
                else {
                    min_val = arr_max + 1;
                    min_val_proc = 0;
                    break_ = 1;
                }
                if (indexes[i] < arr_size / proc_n) {
                    // printf("process #%d tries to receive a value from process #%lu with tag %lu\n", rank, i, index);
                    MPI_Recv(&val, 1, MPI_INT, i, index, MPI_COMM_WORLD, NULL);
                    // printf("Received %d from process #%lu\n", val, i);
                    if (val <= min_val) {
                        min_val = val;
                        min_val_proc = i;
                    }
                    received[i] = 1;
                    break_ = 0;
                } else {
                    received[i] = 0;
                }
            }

            // printf("min_val: %d, process #%d\n", min_val, min_val_proc);
            // swap(&arr[index++], &arr[min_val_proc * arr_size_proc + (indexes[min_val_proc]++)]);
            // printf("swaped\n");
            // printf("index: %lu\n", min_val_proc * arr_size_proc + indexes[min_val_proc]);
            new_arr[index++] = min_val;
            ++indexes[min_val_proc];
            // printf("sorted array: ");
            // for (size_t i = 0; i < index; ++i) {
            //     printf("%d ", new_arr[i]);
            // }
            // printf("\n");

            // Send response
            for (size_t i = 1; i < (size_t) proc_n; ++i) {
                if (received[i]) {
                    if (min_val_proc == i) {
                        // printf("sending 1 to process #%lu with tag %lu\n", i, index - 1);
                        MPI_Ssend(&one, 1, MPI_INT, i, index - 1, MPI_COMM_WORLD);
                    } else {
                        // printf("sending 0 to process #%lu with tag %lu\n", i, index - 1);
                        MPI_Ssend(&zero, 1, MPI_INT, i, index - 1, MPI_COMM_WORLD);
                    }
                }
            }

            if (break_) break;
        }
    } else {
        size_t index = 0;
        size_t arr_size_proc = arr_size / proc_n;
        size_t begin = arr_size_proc * rank;

        int change;

        size_t tag = 0;

        while (index < arr_size_proc) {
            // printf("process #%d sends %d to process #0\n", rank, arr[begin + index]);
            MPI_Ssend(&arr[begin + index], 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
            // printf("process #%d tries to receive change from process #0\n", rank);
            MPI_Recv(&change, 1, MPI_INT, 0, tag++, MPI_COMM_WORLD, NULL);
            // printf("process #%d change: %d\n", rank, change);
            index += change;
        }
    }

    return new_arr;
}