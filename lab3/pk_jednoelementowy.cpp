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

mutex mutexKonsoli;

//Monitor - pojemnik przechowujący dane i zapewniający
//synchronizację wątków
template <typename TypDanych>
class Monitor
{
    private:
        TypDanych dane;
        bool saDane; //czy dane są aktualne

        mutex mutexDane;
        //umożliwia czekanie na nowe dane
        condition_variable warunekSaNoweDane;
        //umożliwia na opróżnienie pojemnika
        condition_variable warunekZabranoDane;
    public:
        Monitor()
        {
            saDane=false; //na początku pojemnik jest pusty
        }
        void zapisz(TypDanych noweDane)
        {
            //blokada uniemożliwia równoczesny odczyt danych
            //musimy użyć unique_lock<> ponieważ zmienna warunku
            //będzie musiała zdjąć blokadę - lock_guard<> się nie nadaje
            unique_lock<mutex> blokada(mutexDane);
            //jeżli pojemnik jest pełny czekamy na zabranie danych
            //(blokada danych jest chwilowo zdejmowana)
            if (saDane) warunekZabranoDane.wait(blokada);
            //pojemnik jest pusty, blokada jest znowu aktywna
            saDane=true;
            dane=noweDane;

            warunekSaNoweDane.notify_one();
            
            //tutaj blokada przestaje istnieć
        }
        TypDanych odczytaj()
        {
            //blokada uniemożliwia równoczesny zapis danych
            unique_lock<mutex> blokada(mutexDane);
            //jeżeli nie ma danych musimy poczekać aż jakieś zostaną zapisane
            //(blokada danych jest chwilowo zdejmowana)
            if (!saDane) warunekSaNoweDane.wait(blokada);
            saDane=false;
            //jeżeli jakiś wątek czeka na opróżnienie pojemnika
            //to powiadamiamy go
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
                pojemnik.zapisz(biezacyZnak);
                {
                    lock_guard<mutex> blokadaKonsoli(mutexKonsoli);
                    cout << "Producent " << numer << " wygenerował znak: " << biezacyZnak << endl;
                }

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
            pojemnik.zapisz(EOF);
            {
                    lock_guard<mutex> blokadaKonsoli(mutexKonsoli);
                    cout << "Producent " << numer << " wygenerował znak: " << (char) EOF << endl;
            }
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
                znak=pojemnik.odczytaj();
                {
                    lock_guard<mutex> blokadaKonsoli(mutexKonsoli);
                    cout << "Konsument " << numer << " odczytał znak: " << znak << endl;
                }
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
    
    int liczbaWatkow= thread::hardware_concurrency();
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
