#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>
// #include <thread>
// #include <chrono>
#include <iostream>
#include <cmath>

using namespace boost;
// using namespace std;

const int LICZBA_PUNKTOW=10;
//w zmiennych globalnych przechowywane są dane potrzebne
//do działania wątku
double a=0.0; //zakres całkowania
double b=M_PI;

//funkcja zawiera zadanie do wykonania w osobnym wątku
void calkaSinusa(int numer, double *wynik) {
    std::cout << "Wątek nr: " << numer << " pracuje..." << std::endl;
    double h = (b - a) / LICZBA_PUNKTOW;
    double s = sin(a);
    for (int i = 1; i < LICZBA_PUNKTOW; i++) {
        //lekkie spowolnienie wątku (wartość w milisekundach)
        //warto wypróbować różne wartości
        this_thread::sleep_for(chrono::milliseconds(10));
        s += 2 * sin(a + i * h);
    }
    s += sin(b);

    *wynik += s * h / 2;
    std::cout << "Wątek nr: " << numer << " się zakończył..." << std::endl;
}

int main(int argc, char *argv[]) {
    
    double wynik = 80.0;

    //tworzenie wątku
    thread watek(calkaSinusa, 100, &wynik);
    
    //główny wątek w między czasie może coś zrobić
    //np. sprawdzić możliwości sprzętu
    std::cout << "Sprzętowa współbieżność: " << thread::hardware_concurrency() << std::endl;
    std::cout << "ID wątku: " << watek.get_id () << std::endl;

    //oczekiwanie na zakończenie wątku
    watek.join();
    std::cout << "wynik= " << wynik << std::endl;
    
    return EXIT_SUCCESS;
}
