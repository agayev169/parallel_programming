#include <stdio.h>

int main() {
    #pragma omp parallel 
    {
        printf("Hello ");
        printf("OpenMP\n");
    }

    return 0;
}

// gcc hello_mess.c