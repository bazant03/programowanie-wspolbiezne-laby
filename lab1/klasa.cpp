#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>
#include <iostream>
#include <cmath>

using namespace boost;

const int LICZBA_PUNKTOW=10;

//klasa reprezentuje zadanie
class CalkaSinusa {
    private:
        //w polach przechowywane są dane potrzebne do działania wątku
        double a, b; //zakres całkowania
        double *wartosc; //wskaźnik do zmiennej, w której
                         //należy zapisać wynik
    public:
        //kostruktor ustawia wartości pól
        CalkaSinusa(double _a, double _b, double *_wartosc) :
            a(_a), b(_b), wartosc(_wartosc) {
            }

        //obiektu będzie można używać jak funkcji dzięki przeciążeniu
        //operatora nawiasy, operator nie posiada parametrów
        void operator()();
};

//treść przeciążonego operatora - czyli zadanie do wykonania
//w osobnym wątku
void CalkaSinusa::operator()() {
    std::cout << "Wątek pracuje..." << std::endl;
    double h = (b - a) / LICZBA_PUNKTOW;
    double s = sin(a);
    for (int i = 1; i < LICZBA_PUNKTOW; i++) {
        //lekkie spowolnienie wątku (wartość w milisekundach)
        //warto wypróbować różne wartości
        this_thread::sleep_for(chrono::milliseconds(1));
        s += 2 * sin(a + i * h);
    }
    s += sin(b);

    *wartosc = s * h / 2;
    std::cout << "Wątek się zakończył..." << std::endl;
}

int main(int argc, char *argv[]) {
    //zmienna w której wątek zapisze wynik
    double wynik=0.0;
    //tworzenie obiektu z zadaniem
    CalkaSinusa calka(0, M_PI, &wynik);
    //tworzenie wątku
    thread watek(calka);

    //główny wątek w między czasie może coś zrobić
    //np. sprawdzić możliwości sprzętu
    std::cout << "Sprzętowa współbieżność: " << thread::hardware_concurrency () << std::endl;
    std::cout << "ID wątku: " << watek.get_id () << std::endl;

    //oczekiwanie na zakończenie wątku
    watek.join();
    std::cout << "wynik= "<< wynik << std::endl;
    return EXIT_SUCCESS;
}
