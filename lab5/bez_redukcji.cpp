#include <iostream>
#include <omp.h>

using namespace std;

int main () {
    int tab[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}, threads;
    long int suma = 0;
    int suma_thr = 0;

    threads=omp_get_num_procs();
    omp_set_num_threads(threads);

    #pragma omp parallel firstprivate(suma_thr) shared(suma)
    {
        #pragma omp for
            for(int i=0;i<16;i++)
                suma_thr+=tab[i];

        #pragma omp critical
            suma+=suma_thr;
    }

    cout << "suma = " << suma << endl;
    return 0;
}
