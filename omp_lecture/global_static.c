#include <stdio.h>
#include <omp.h>

int tid, tprivate, rprivate;
#pragma omp threadprivate(tprivate)

int main() {
  // On interdit explicitement les threads dynamiques
  omp_set_dynamic(0);

  printf("Parallel region 1\n");
  #pragma omp parallel private(tid, rprivate)
  {
    tid = omp_get_thread_num();
    tprivate = tid;
    rprivate = tid;
    printf("Thread %d: tprivate=%d rprivate=%d\n", tid, tprivate, rprivate);
  }

  printf("Parallel region 2\n");
  #pragma omp parallel private(tid, rprivate)
  {
    tid = omp_get_thread_num();
    printf("Thread %d: tprivate=%d rprivate=%d\n", tid, tprivate, rprivate);
  }
  return 0;
}