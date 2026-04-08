#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>

using namespace std;

const int LICZBA_PUNKTOW=10;

//klasa reprezentuje zadanie
class calkowanie_m_prostokatow {
    private:
        //w polach przechowywane są dane potrzebne do działania wątku
        double a, b; //zakres całkowania
        double *wartosc; //wskaźnik do zmiennej, w której należy
                         //zapisać wynik
    public:
        //kostruktor ustawia wartości pól
        calkowanie_m_prostokatow(double _a, double _b, double *_wartosc) :
            a(_a), b(_b), wartosc(_wartosc) {}

        const double get_a() {
            return this->a;
        }

        const double get_b() {
            return this->b;
        }

        void set_wartosc(const double _wartosc) {
            *(this->wartosc) = _wartosc;
        }

};

//treść przeciążonego operatora - czyli zadanie do wykonania
//w osobnym wątku

int main(int argc, char *argv[]) {
    //zmienna w której wątek zapisze wynik
    double wynik=0.0;
    //przedzial calkowania
    double a = 0.0;
    double b = 1.0;
    double wynik_dokladny;
    //tworzenie obiektu z zadaniem
    calkowanie_m_prostokatow calka(a, b, &wynik);
    //tworzenie wątku
    thread watek([](calkowanie_m_prostokatow &calka, int numer_watku){    
        cout << "Wątek nr " << numer_watku << " pracuje..." << endl;
        double h = (calka.get_b() - calka.get_a()) / LICZBA_PUNKTOW;
        double s = 0.0;
        for (int i = 0; i < LICZBA_PUNKTOW; i++) {
            //lekkie spowolnienie wątku (wartość w milisekundach)
            this_thread::sleep_for(chrono::milliseconds(10));
            // f(x)=x
            s += h * (calka.get_a() + i * h);
        }

        calka.set_wartosc(s);
        cout << "Wątek nr " << numer_watku << " się zakończył..." << endl;
    }, ref(calka), 100);

    //główny wątek w między czasie może coś zrobić
    //np. sprawdzić możliwości sprzętu
    cout << "Sprzętowa współbieżność: " <<
        thread::hardware_concurrency () << endl;
    cout << "ID wątku: " << watek.get_id () << endl;
    // i policzyć dokładną wartosc całki z funkcji f(x)=x
    wynik_dokladny=a*(b-a)+(b-a)*(b-a)/2.0;
    cout << "wynik dokladny policzony przez watek glowny = " << wynik_dokladny << endl;

    //oczekiwanie na zakończenie wątku
    watek.join();
    cout << "wynik calkowania numerycznego policzony przez watek dodatkowy = "<< wynik << endl;
    cout << "blad = " << wynik_dokladny-wynik << ",\tblad w % = " << (wynik_dokladny-wynik)/wynik_dokladny*100 << endl;
    return EXIT_SUCCESS;
} 
