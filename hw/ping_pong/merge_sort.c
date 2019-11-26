#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));

    size_t arr_size = 1000;
    int arr_max     = 1000000;

    int arr[arr_size];
    for (size_t i = 0; i < arr_size; ++i) {
        arr[i] = rand() % (arr_max + 1);
    }

    for (size_t i = 0; i < 10; ++i) {
        printf("%d ", arr[i]);
    }
    print("\n");
}