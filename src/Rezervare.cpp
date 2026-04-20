#include "Rezervare.h"

using std::cout;

Rezervare::Rezervare(const Film* film, Sala* sala, int rand, int loc) : film(film), sala(sala), rand(rand), loc(loc){
    this -> sala -> rezervaLoc(rand, loc);
} 

void Rezervare::afisareDetalii() const{
    std::cout << "\n--------------------------\n";
    std::cout << "      TICKET CINEMA       \n";
    std::cout << "--------------------------\n";
    std::cout << "Film: " << film->getTitlu() << " (" 
              << (film->getTip() == TipFilm::Film2D ? "2D" : "3D") << ")\n";
    std::cout << "Sala: " << sala->GetNume() << "\n";
    std::cout << "Loc:  Randul " << rand << ", Locul " << loc << "\n";
    std::cout << "--------------------------\n";
}