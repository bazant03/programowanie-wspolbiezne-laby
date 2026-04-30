#include <iostream>
#include <omp.h>
using namespace std;

int main () {
    int id;
    #pragma omp parallel shared(id)
    {
        id = omp_get_thread_num();
        int nthreads = omp_get_max_threads();
        
        #pragma omp critical
            cout << "Wątek nr " << id << " z " << nthreads << endl;
    }
    return 0;
}
