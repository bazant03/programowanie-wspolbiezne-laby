#include <thread>
#include <mutex>
#include <random>
#include <iostream>
#include <set>
#include <vector>
#include <thread>

#include "semafor.h"

using namespace std;

const int POJEMNOSC_HANGARU=5;
const int LICZBA_SAMOLOTOW=10;

random_device rd;
minstd_rand generator(rd());
uniform_int_distribution<int> zakres(1, 3);

mutex mutexKonsoli;

class Hangar;
class Samolot;

class Lotnisko {
    private:
        bool pasWolny;
        Hangar &hangar;
        Semafor semafor;

    public:

        Lotnisko(Hangar &h) : pasWolny(true), hangar(h){
        }
        void laduj(Samolot &s);
        void kolujDoHangaru(Samolot &s);
        void opuscHangar(Samolot &s);
        void startuj(Samolot &s);
};


class Samolot {
    public:

        Samolot(string n, Lotnisko &l) : nazwa(n), lotnisko(l) {
        }
        string nazwa;

        void operator()() {
            this_thread::sleep_for(chrono::seconds(zakres(generator)));
            lotnisko.laduj(*this);
            // postoj przy hali przylotow
            this_thread::sleep_for(chrono::seconds(zakres(generator)));
            lotnisko.kolujDoHangaru(*this);
            // postoj w hangarze
            this_thread::sleep_for(chrono::seconds(zakres(generator)));
            lotnisko.opuscHangar(*this);
            // postoj przy hali odlotow
            this_thread::sleep_for(chrono::seconds(zakres(generator)));
            lotnisko.startuj(*this);
        }

    private:
        Lotnisko &lotnisko;
};

class Hangar {
    private:
        unsigned rozmiarHangaru;
        set<string> samoloty;
        Semafor semafor;
        Semafor semaforPomocniczy;

    public:

        Hangar(int n) : rozmiarHangaru(n), semafor(n) {}

        void parkuj(Samolot &s) {
            semafor.czekaj();
            if (samoloty.size() < rozmiarHangaru) {
                semaforPomocniczy.czekaj();
                samoloty.insert(s.nazwa);
                semaforPomocniczy.sygnalizuj();
                string info = "Samolot " + 
                                s.nazwa + 
                                " wjeżdża do hangaru: " +
                                to_string(samoloty.size()) +
                                " w hangarze\n";
                {
                    lock_guard<mutex> blokada(mutexKonsoli);
                    cout << info;
                }
            } else {
                cout << "Hangar pelny\n";
            }
            semafor.sygnalizuj();
        }

        void opusc(Samolot &s) {
            semafor.czekaj();
            if (samoloty.count(s.nazwa) == 1) {
                semaforPomocniczy.czekaj();
                samoloty.erase(s.nazwa);
                semaforPomocniczy.sygnalizuj();
                string info = "Samolot " + 
                                s.nazwa + 
                                " wyjeżdża z hangaru: " +
                                to_string(samoloty.size()) +
                                " w hangarze\n";
                {
                    lock_guard<mutex> blokada(mutexKonsoli);
                    cout << info;
                }
            } else {
                cout << "W hangarze nie ma tego samolotu\n";
            }
            semafor.sygnalizuj();
        }
};

void Lotnisko::laduj(Samolot &s) {
    semafor.czekaj();
    if (pasWolny) {
        pasWolny = false;
        
        string info = "Samolot " + s.nazwa + " ląduje\n";
        {
            lock_guard<mutex> blokada(mutexKonsoli);
            cout << info;
        }
        this_thread::sleep_for(chrono::seconds(zakres(generator)));
        
        info = "Samolot " + s.nazwa + " wylądował\n";
        {
            lock_guard<mutex> blokada(mutexKonsoli);
            cout << info;
        }
        pasWolny = true;
    } else {
        cout << "Zderzenie na pasie startowym\n";
    }
    semafor.sygnalizuj();
}

void Lotnisko::startuj(Samolot &s) {
    semafor.czekaj();
    if (pasWolny) {
        pasWolny = false;
        
        string info = "Samolot " + s.nazwa + " startuje\n";
        {
            lock_guard<mutex> blokada(mutexKonsoli);
            cout << info;
        }
        this_thread::sleep_for(chrono::seconds(zakres(generator)));
        
        info = "Samolot " + s.nazwa + " wystartowal\n";
        {
            lock_guard<mutex> blokada(mutexKonsoli);
            cout << info;
        }
        pasWolny = true;
    } else {
        cout << "Zderzenie na pasie startowym\n";
    }
    semafor.sygnalizuj();
}

void Lotnisko::kolujDoHangaru(Samolot &s) {
    hangar.parkuj(s);
}

void Lotnisko::opuscHangar(Samolot &s) {
    hangar.opusc(s);
}

int main(int argc, char** argv) {
    Hangar hangar(POJEMNOSC_HANGARU);
    Lotnisko lotnisko(hangar);

    vector<thread> watki;

    for (int i = 0; i < LICZBA_SAMOLOTOW; ++i) {
        string napis = "Rownolegle Linie Lotnicze "+to_string(i);
        watki.emplace_back(Samolot(napis, lotnisko));
    }

    for (thread &w: watki) w.join();

    return EXIT_SUCCESS;
}
