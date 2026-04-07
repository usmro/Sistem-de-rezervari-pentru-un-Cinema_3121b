#include <iostream>
#include "Film.h"

using std::cout;
using std::endl;

int main() {
    cout << "SISTEM DE REZERVARI PENTRU UN CINEMA" << endl;

    Film film1("Dune: Part Two", 155, TipFilm::Film2D);
    Film film2("Avatar: The Way of Water", 192, TipFilm::Film3D);

    cout << "FILME DISPONIBILE:" << endl;
    film1.afisare();
    film2.afisare();

    return 0;
}