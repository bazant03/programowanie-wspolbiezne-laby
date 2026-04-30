#include <iostream>
#include <omp.h>

using namespace std;

int main() {
    omp_set_num_threads(5);
    #pragma omp parallel 
    {
        #pragma omp critical
        { 
            cout<<"Hello " << "World" << endl;
        }
    }
    
    #pragma omp parallel num_threads(3)
    {
        #pragma omp critical
        { 
            cout<<"Hello " << "World2" << endl;
        }
    }

    return 0;
}
