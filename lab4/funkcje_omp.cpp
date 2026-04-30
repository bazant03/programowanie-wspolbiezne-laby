#include <iostream>
#include <omp.h>

using namespace std;

int main() {

    // num_threads - dostepne w danym bloku
    // max_threads - zadeklarowana liczba watkow
    // max_procs - faktyczna liczba watkow w cpu
    
    omp_set_num_threads(omp_get_num_procs());

    cout << "Blok sekwencyjny:" << endl;
    cout << "Watek " << omp_get_thread_num() << " z " << omp_get_max_threads() << endl;
    cout << endl << "Blok rownolegly:" << endl;
    #pragma omp parallel 
    {
        #pragma omp critical
        { 
            cout << "Watek " << omp_get_thread_num() << " z " << omp_get_num_threads() << endl;
        }
    }
    
    return 0;
}
