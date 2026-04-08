#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>
//mutex = mutual exclusion - umożliwia szeregowanie dostępu do wspólnych zasobów
#include <mutex>

using namespace std;

const int OPOZNIENIE=100;
int licznik=50;

//deklaracja mutexu dla licznika, który jest wspólnym zasobem
mutex licznikMutex;

class Watek
{
    private:
        int numer;
    public:
        Watek(int _numer): numer(_numer) {}
        void operator()()
        {
            while (true)
            {
                {
                    //tworzymy blokadę, który uniemożliwi równoczesny dostęp do
                    //wspólnej zmiennej licznik (korzystamy z wcześniej utworzonego mutexu))
                    lock_guard<mutex> blokada(licznikMutex);
                    if (licznik<=0) break;
                    //przy okazji wyklucza równoczesny dostęp do standardowego wyjścia
                    cout<<"w: "<<numer<<" l: "<<licznik<<endl;
                    --licznik;
                    //tutaj blokada przestaje istnieć - kończy się zakres zmiennej blokada
                }
                //"zamiast pracować ja wolę leniuchować"
                this_thread::sleep_for(chrono::microseconds(OPOZNIENIE));
            }
        }
};

int main(int argc, char** argv)
{
    thread w1(Watek(1));
    thread w2(Watek(2));
    w1.join();
    w2.join();
    return (EXIT_SUCCESS);
}
