#include <iostream>
#include <omp.h>
using namespace std;

int main () {
    int id;
    int nthreads = omp_get_max_threads();
    #pragma omp parallel private(id)
    {
        id = omp_get_thread_num();
        #pragma omp critical
            cout << "Wątek nr " << id << " z " << nthreads << endl;
    }
    return 0;
}
