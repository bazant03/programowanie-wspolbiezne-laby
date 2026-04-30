#include <iostream>
#include <omp.h>
using namespace std;

int main () {
    int id;
    int nthreads;

    #pragma omp parallel private(id) num_threads(2)
    {
        id = omp_get_thread_num();

        #pragma omp single
            nthreads = omp_get_num_threads();

        #pragma omp for
            for(int i=0; i<32; ++i)
                #pragma omp critical
                    cout << "Obrót pętli " << i << ", wątek nr " << id << " z " << nthreads << endl;
    }
    return 0;
}
