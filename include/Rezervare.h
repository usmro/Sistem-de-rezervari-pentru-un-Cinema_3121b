#pragma once
#include "Film.h"
#include "Sala.h"
#include <iostream>

class Rezervare{
    private:
        const Film* film;
        Sala* sala;
        int rand, loc;

    public:
        Rezervare(const Film* film, Sala* sala, int rand, int loc);

        void afisareDetalii() const;
};

