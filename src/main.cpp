#include <iostream>
#include "Film.h"
#include "Sala.h"
#include "Rezervare.h"
#include "Cinematograf.h"

using std::cout;
using std::endl;

int main() {
    
    Cinematograf cinemaCity("Cinema City Suceava");
    
    Sala sala1("Sala IMAX", 5, 10);
    Sala sala2("Sala 4DX", 4, 8);

    cinemaCity.adaugaSala(&sala1);
    cinemaCity.adaugaSala(&sala2);

    cinemaCity.afisareSali();

    cout << "SISTEM DE REZERVARI PENTRU UN CINEMA" << endl;

    Film film1("Dune: Part Two", 155, TipFilm::Film2D);
    sala1.afisareLocuri();

    try {
        std::cout << "Procesam prima rezervare...\n";
        Rezervare rez1(&film1, &sala1, 3, 5); 
        rez1.afisareDetalii();

        std::cout << "\nProcesam a doua rezervare...\n";
        Rezervare rez2(&film1, &sala1, 3, 6);
        rez2.afisareDetalii();

        std::cout << "\nProcesam o rezervare GRESITA (acelasi loc)...\n";
        Rezervare rez3(&film1, &sala1, 3, 5);
        rez3.afisareDetalii(); 

    } catch (const std::exception& e) {
        std::cerr << "EROARE SISTEM: " << e.what() << "\n";
    }

    return 0;
}