#include <stdio.h>
#include <omp.h>
#include <unistd.h>

#define MAX 32

int main() {
    double t;
    int i;

    t=omp_get_wtime();

    // bardzo czasochłonne obliczenia :)
    for(i=0; i<MAX; i++)
        sleep(1);

    t=omp_get_wtime()-t;

    //printf("Czas obliczeń = %le\n",t);
    printf("Czas obliczeń = %7.4f\n",t);

    return 0;
}
