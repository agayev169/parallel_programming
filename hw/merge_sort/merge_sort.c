#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int is_proc_nb_valid();

int main(int argc, char *argv[]) {
    srand(time(NULL));

    size_t arr_size = 1000;
    int arr_max     = 1000000;

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