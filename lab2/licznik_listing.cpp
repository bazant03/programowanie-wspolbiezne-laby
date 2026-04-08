//wejscie/wyjscie w C++
#include <iostream>
//obsluga watkow
#include <thread>

using namespace std;

//czas wykonania zadania przez watek (w mikrosekundach)
const int OPOZNIENIE=100;

//liczba zadan do wykonania przez watki
//***licznik jest WSPÓLNY dla wszystkich wątków***
int licznik=50;

//klasa Watek zawiera kod wykonywany przez watki
class Watek
{
    private:
        //watkom nadajemy numry aby latwo je rozroznic
        int numer;
    public:
        //konstruktor ustawia tylko numer watku
        Watek(int _numer): numer(_numer) {}
        //kod watku nalezy umiescic w przeciazonym operatorze ()
        void operator()()
        {
            //dopoki jest cos do zrobienia
            while (licznik>0)
            {
                //wyswietlamy numer watku i licznik
                cout<<"w: "<<numer<<" l: "<<licznik<<endl;
                --licznik;
                //"zamiast pracowac ja wole leniuchowac"
                this_thread::sleep_for(chrono::microseconds(OPOZNIENIE));
            }
        }
};

int main(int argc, char** argv)
{
    //tworzymy watki
    thread w1(Watek(1));
    thread w2(Watek(2));
    //czekamy na ich zakonczenie
    w1.join();
    w2.join();
    return (EXIT_SUCCESS);
} 
