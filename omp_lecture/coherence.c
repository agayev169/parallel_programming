#include <stdio.h>

int main() {
    int done = 0;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            while (done == 0)
                #pragma omp critical(done)
                {
                    if (done == 0) {
                        printf("Not Done\n");
                    }
                }
        }

        #pragma omp section
        {
            #pragma omp critical(done)
            {
                done = 1;
                printf("Done\n");
            }
        }
    }
    return 0;
}