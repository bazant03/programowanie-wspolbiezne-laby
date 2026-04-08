class Monitor {
private:
    TypDanych dane[ROZMIAR_BUFORA + 1];
    int poczatek;
    int koniec;

    bool kolejkaPusta() {
        return (poczatek == koniec);
    }

    bool kolejkaPelna() {
        return ((koniec + 1) % (ROZMIAR_BUFORA + 1) == poczatek);
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
private:
    TypDanych dane[ROZMIAR_BUFORA + 1];
    int poczatek;
    int koniec;

    bool kolejkaPusta() {
        return (poczatek == koniec);
    }

    bool kolejkaPelna() {
        return ((koniec + 1) % (ROZMIAR_BUFORA + 1) == poczatek);
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
