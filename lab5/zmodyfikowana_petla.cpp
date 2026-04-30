#include <iostream>
#include <omp.h>

using namespace std;

int main () {
    int id;
	int maxtreads;
    int nthreads;

    #pragma omp parallel private(id) num_threads(2)
    {
        id = omp_get_thread_num();

        #pragma omp single
		{
			maxtreads=omp_get_max_threads();
            nthreads = omp_get_num_threads();
//			cout << "nthreads=" << nthreads << endl;
//			cout << "maxtreads=" << maxtreads << endl;
		}

//        #pragma omp critical
//            cout << "nthreads=" << nthreads << ", maxtreads= " << maxtreads << endl;

        #pragma omp for schedule(static, 4)
        for(int i=0; i<32; ++i)
            #pragma omp critical
                cout << "Obrót pętli " << i << ", wątek nr " << id << " z " << nthreads << endl;
    }

    return 0;
}
