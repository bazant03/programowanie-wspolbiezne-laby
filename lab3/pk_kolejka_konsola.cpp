#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>

//umożliwiają oczekiwanie na spełnienie określonego warunku
#include <condition_variable>
//generator liczb losowych
#include <random>

using namespace std;

//czas przez jaki producent będzie generował znaki
const int CZAS_PRACY=5;
const int ROZMIAR_BUFORA=8;

mutex mutexKonsoli;

//Monitor - pojemnik przechowujący dane i zapewniający
//synchronizację wątków
template <typename TypDanych>
class Monitor
{
    private:
        TypDanych dane[ROZMIAR_BUFORA + 1];
        int poczatek = 0;
        int koniec = 0;

        bool kolejkaPusta() {
            return (poczatek == koniec);
        }

        bool kolejkaPelna() {
            return (poczatek - koniec == 1 || koniec - poczatek == ROZMIAR_BUFORA);
        }

        void dodajDoKolejki(TypDanych nowaDana) {
            dane[koniec] = nowaDana;
            ++koniec;
            koniec %= (ROZMIAR_BUFORA + 1);
        }

        TypDanych zabierzZKolejki() {
            TypDanych wynik = dane[poczatek];
            ++poczatek;
            poczatek %= (ROZMIAR_BUFORA + 1);
            return wynik;
        }

        mutex mutexDane;
        //umożliwia czekanie na nowe dane
        condition_variable warunekSaNoweDane;
        //umożliwia na opróżnienie pojemnika
        condition_variable warunekZabranoDane;
    public:
        Monitor() {}
        
        void zapisz(TypDanych noweDane, int numer)
        {
            //blokada uniemożliwia równoczesny odczyt danych
            //musimy użyć unique_lock<> ponieważ zmienna warunku
            //będzie musiała zdjąć blokadę - lock_guard<> się nie nadaje
            unique_lock<mutex> blokada(mutexDane);
            //jeżli pojemnik jest pełny czekamy na zabranie danych
            //(blokada danych jest chwilowo zdejmowana)
            while (kolejkaPelna()) warunekZabranoDane.wait(blokada);
            //pojemnik jest pusty, blokada jest znowu aktywna
            dodajDoKolejki(noweDane);
            cout << "Producent " << numer << " wygenerował znak: " << (char)noweDane << endl;
            warunekSaNoweDane.notify_one();
            //tutaj blokada przestaje istnieć
        }
        TypDanych odczytaj(int numer)
        {
            //blokada uniemożliwia równoczesny zapis danych
            unique_lock<mutex> blokada(mutexDane);
            //jeżeli nie ma danych musimy poczekać aż jakieś zostaną zapisane
            //(blokada danych jest chwilowo zdejmowana)
            while (kolejkaPusta()) warunekSaNoweDane.wait(blokada);
            //jeżeli jakiś wątek czeka na opróżnienie pojemnika
            //to powiadamiamy go
            TypDanych dane = zabierzZKolejki();
            cout << "Konsument " << numer << " odczytał znak: " << dane << endl;
            warunekZabranoDane.notify_one();
            return dane;
            //tutaj blokada przestaje istnieć
        }
};

class Producent
{
    private:
        int numer;
        //czas przez jaki pracował producent
        int czas;
        //referencja do wspólnego pojemnika na dane
        Monitor< char >& pojemnik;
        //bieżący znak
        char biezacyZnak;

        //generator liczb losowych
        minstd_rand generator; //algorytm generatora liczb pseudolosowych
        uniform_int_distribution< char > rozkladZnaki; //rozkład liczb pseudolosowych
        uniform_int_distribution< int > rozkladOpoznienie;
    public:
        //tworzenie producenta generującego znaki [a,z]
        Producent(int _numer, Monitor < char >& _pojemnik):
            numer(_numer),
            czas(0), 
            pojemnik(_pojemnik),
            generator(random_device()()),
            rozkladZnaki('a', 'z'),
            rozkladOpoznienie(50, 400) {
                biezacyZnak=rozkladZnaki(generator);
            }

        //kod wykonywany w wątku
        void operator()()
        {
            while (czas<CZAS_PRACY)
            {
                //na początku generujemy kolejne znaki
                //(dla sprawdzenia synchronizacji)
                pojemnik.zapisz(biezacyZnak, numer);

                //kolejny znak od &apos;a&apos; do &apos;z&apos;
                biezacyZnak=rozkladZnaki(generator);

                //potem dla urozmaicenia
                //zamiast kolejnych można zpisywać losowe znaki
                //pojemnik.zapisz(rozklad(generator));

                //przerwa w pracy
                this_thread::sleep_for(chrono::milliseconds(rozkladOpoznienie(generator)));
                ++czas;
            }
            //znak "koniec pliku" powoduje zakończenie konsumenta
            pojemnik.zapisz(EOF, numer);
        }
};

class Konsument
{
    private:
        int numer;
        //referencja do wspólnego pojemnika
        Monitor< char >& pojemnik;
    
    public:
        //tworzenie konsumenta
        Konsument(int _numer, Monitor< char >& _pojemnik): numer(_numer), pojemnik(_pojemnik) {}

        //kod wykonywany w wątku
        void operator()()
        {
            char znak;
            do 
            {
                znak=pojemnik.odczytaj(numer);
            }
            while (znak!=EOF);
        }
};

int main(int argc, char** argv)
{
    random_device r;
    minstd_rand generator(r());
    //tworzenie wspólnego pojemnika dla konsumenta i producenta
    Monitor< char > pojemnik;
    //tworzenie producenta i konsumenta
    
    int liczbaWatkow = thread::hardware_concurrency();
    vector<thread> produceni(liczbaWatkow);
    vector<thread> konsumenci(liczbaWatkow);

    //tworzenie watkow
    for (int i=0; i<liczbaWatkow; ++i)
    {
        produceni[i] = thread(Producent(i, pojemnik));
        konsumenci[i] = thread(Konsument(i, pojemnik));
    }
    
    //czekanie na zakończenie wątków
    for (int i=0; i<liczbaWatkow; ++i)
    {
        produceni[i].join();
        konsumenci[i].join();
    }
    
    return (EXIT_SUCCESS);
}
