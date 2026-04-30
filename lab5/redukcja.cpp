#include <iostream>
#include <omp.h>

using namespace std;

int main () {
    int tab[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}, threads;
    long int suma = 0;
    
    threads=omp_get_num_procs();
    omp_set_num_threads(threads);
    
    #pragma omp parallel
    {
        #pragma omp for reduction(+:suma)
        for (int i=0; i<16; ++i)
            suma += tab[i];
    }
    
    cout << "suma = " << suma << endl;
    return 0;
}
