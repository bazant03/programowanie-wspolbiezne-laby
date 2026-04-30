#include <thread>
#include <mutex>
#include <random>
#include <iostream>
#include <memory>
#include <vector>

#include "semafor.h"

using namespace std;

//czas jedzenia w ms
const int MAX_OPOZNIENIE = 1000;

random_device rd;
minstd_rand generator(rd());
uniform_int_distribution<int> rozklad(0, MAX_OPOZNIENIE);

const int LICZBA_FILOZOFOW = 5;
const int LICZBA_POSILKOW = 3;

mutex mutexKonsoli;

class Kelner {
    private:
        Semafor semafor;
    
    public:
        Kelner(int liczba_filozofow): semafor(liczba_filozofow) {}

        void zapytajOPozwolenie() {
            semafor.czekaj();
        }

        void sygnalizujZwolnienie() {
            semafor.sygnalizuj();
        }
};

class Widelec;

class Filozof {
    private:
        int numer;
        int lewyWidelec;
        int prawyWidelec;
        int zjedzonychPosilkow;
        Kelner &kelner;
        vector<Widelec*> &widelce;
    
        public:
        Filozof(int, Kelner&, vector<Widelec*>&);
        int pobierzNumer();
        void operator()();
};

class Widelec {
    private:
        int numer;
        Semafor semafor;

    public:
        Widelec(int _numer): numer(_numer) {}
        ~Widelec() {}
        
        void podniesWidelec() {
            semafor.czekaj();
            // {
            //     lock_guard<mutex> blokada(mutexKonsoli);
            //     cout << "Filozof " << f.pobierzNumer() << " podnosi " << lp << " widelec" << endl;
            // }
        }

        void odlozWidelec() {
            semafor.sygnalizuj();
            // {
            //     lock_guard<mutex> blokada(mutexKonsoli);
            //     cout << "Filozof " << f.pobierzNumer() << " odklada " << lp << " widelec" << endl;
            // }
        }

        bool probujPodniescWidelec() {
            return semafor.probuj_czekac();
        }
};

Filozof::Filozof(int n, Kelner &_kelner, vector<Widelec*> &_widelce): 
    numer(n), zjedzonychPosilkow(0), kelner(_kelner), widelce(_widelce) {
    lewyWidelec = n;
    prawyWidelec = (n > 0) ? n - 1 : LICZBA_FILOZOFOW - 1;
}

int Filozof::pobierzNumer() {
    return numer;
}

void Filozof::operator()() {
    while (zjedzonychPosilkow < LICZBA_POSILKOW) {

        kelner.zapytajOPozwolenie();
        
        widelce.at(lewyWidelec)->podniesWidelec();
        widelce.at(prawyWidelec)->podniesWidelec();

        int czasPosilku = rozklad(generator);
        {
            lock_guard<mutex> blokada(mutexKonsoli);
            cout << "Filozof " << numer << " je "
                << zjedzonychPosilkow << " posilek przez najblizsze "
                << czasPosilku << " ms" << endl;
        }
        //filozof je
        this_thread::sleep_for(chrono::milliseconds(czasPosilku));
        
        {
            lock_guard<mutex> blokada(mutexKonsoli);
            cout << "Filozof " << numer << " skonczyl jesc "
                << zjedzonychPosilkow << " posilek" << endl;
        }

        widelce.at(lewyWidelec)->odlozWidelec();
        widelce.at(prawyWidelec)->odlozWidelec();
        
        kelner.sygnalizujZwolnienie();

        ++zjedzonychPosilkow;
        //filozof rozmyśla między posiłkami
    }
    cout << "Filozof " << numer << " sie najadl" << endl;
}

int main(int argc, char** argv) {

    // Ogranicza konkurencje filozofow
    Kelner kelner = Kelner(LICZBA_FILOZOFOW - 1);  

    //tworzenie widelcow i filozofow
    vector<Widelec*> widelce;
    vector<thread> filozofowie;

    for (int i = 0; i < LICZBA_FILOZOFOW; ++i) 
        widelce.emplace_back(new Widelec(i));
    
    for (int i = 0; i < LICZBA_FILOZOFOW; ++i)
        filozofowie.emplace_back(Filozof(i, kelner, widelce));

    //czekanie na zakonczenie
    for (thread &f:filozofowie) f.join();

    //zwolnienie zasobow
    for (int i = 0; i < LICZBA_FILOZOFOW; i++)
        delete widelce[i];

    return (EXIT_SUCCESS);
}
