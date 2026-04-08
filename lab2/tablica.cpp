#include <stdlib.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

const int LICZBA_WATKOW = 5;
const int ZNAKOW_DO_ZAPISANIA = 1024;
const int OPOZNIENIE = 100;

mutex znakMutex;

class Watek {
    private:
        //znak który będzie zapisywany przez wątek
        char znak;
        //do łańcucha, do którego mamy tu referencję
        string &lancuch;
    public:

        Watek(char _znak, string &_lancuch) : znak(_znak), lancuch(_lancuch) { }

        void operator()() {
            //pętla wypełniająca cały łańcuch jednym znakiem

            lock_guard<mutex> blokada(znakMutex);
            for (int i = 0; i < ZNAKOW_DO_ZAPISANIA; ++i) {
                this_thread::sleep_for(chrono::microseconds(OPOZNIENIE));
                lancuch[i] = znak;
            }
        }
};

int main(int argc, char** argv) {
    //łańcuch, do którego zapisywane są dane
    //wszystkie wątki zapisują do tego samego łańcucha
    string lancuch;
    lancuch.resize(ZNAKOW_DO_ZAPISANIA);

    //tworzenie wektora wątków + tworzenie wątków
    vector<thread> watki;
    for (unsigned int i = 0; i < LICZBA_WATKOW; ++i)
        watki.push_back(thread(Watek('!' + i, lancuch)));

    //oczekiwanie na zakończenie wszystkich wątków
    for (thread& w: watki) w.join();

    cout << lancuch << endl;

    return (EXIT_SUCCESS);
}
