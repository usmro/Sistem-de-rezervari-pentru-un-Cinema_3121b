#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Sala.h"

class Cinematograf {
private:
    std::string nume;
    std::vector<Sala*> sali; 

public:
    Cinematograf(std::string nume);
    void adaugaSala(Sala* sala);
    void afisareSali() const;
    std::string getNume() const;
};