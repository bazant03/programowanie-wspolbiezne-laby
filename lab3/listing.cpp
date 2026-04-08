#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

//umożliwiają oczekiwanie na spełnienie określonego warunku
#include <condition_variable>
//generator liczb losowych
#include <random>

using namespace std;

//czas przez jaki producent będzie generował znaki
const int CZAS_PRACY=5;

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
            //jeżeli na dane czeka inny wątek powiadamiamy go
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
        //czas przez jaki pracował producent
        int czas;
        //referencja do wspólnego pojemnika na dane
        Monitor< char >& pojemnik;
        //bieżący znak
        char biezacyZnak;

        //generator liczb losowych
        minstd_rand generator; //algorytm generatora liczb pseudolosowych
        uniform_int_distribution< char > rozkladZnaki; //rozkład liczb pseudolosowych
    public:
        //tworzenie producenta generującego znaki [a,z]
        Producent(Monitor < char >& _pojemnik):
            czas(0), pojemnik(_pojemnik), biezacyZnak('a'),generator(random_device()()),rozkladZnaki(&apos;a&apos;,&apos;z&apos;) { }

        //kod wykonywany w wątku
        void operator()()
        {
            while (czas<CZAS_PRACY)
            {
                //na początku generujemy kolejne znaki
                //(dla sprawdzenia synchronizacji)
                pojemnik.zapisz(biezacyZnak);
                //kolejny znak od &apos;a&apos; do &apos;z&apos;
                biezacyZnak=(biezacyZnak+1-97)%25+97;

                //potem dla urozmaicenia
                //zamiast kolejnych można zpisywać losowe znaki
                //pojemnik.zapisz(rozklad(generator));

                //przerwa w pracy
                this_thread::sleep_for(chrono::seconds(1));
                ++czas;
            }
            //znak "koniec pliku" powoduje zakończenie konsumenta
            pojemnik.zapisz(EOF);
        }
};

class Konsument
{
    private:
        //referencja do wspólnego pojemnika
        Monitor< char >& pojemnik;
    public:
        //tworzenie konsumenta
        Konsument(Monitor< char >& _pojemnik): pojemnik(_pojemnik) {}

        //kod wykonywany w wątku
        void operator()()
        {
            char znak=pojemnik.odczytaj();
            while (znak!=EOF)
            {
                cout<<znak<<endl;
                znak=pojemnik.odczytaj();
            }
        }
};

int main(int argc, char** argv)
{
    random_device r;
    minstd_rand generator(r()) ;
    //tworzenie wspólnego pojemnika dla konsumenta i producenta
    Monitor< char > pojemnik;
    //tworzenie producenta i konsumenta
    Konsument konsument(pojemnik);
    Producent producent(pojemnik);
    //tworzenie wątków
    thread wk(konsument);
    thread wp(producent);
    //czekanie na zakończenie wątków
    wp.join();
    wk.join();
    return (EXIT_SUCCESS);
}
