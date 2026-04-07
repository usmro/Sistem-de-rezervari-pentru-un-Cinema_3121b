#include "Film.h"

using std::string;

Film::Film(string titlu, int durataminute, TipFilm tip) : titlu(titlu), durataminute(durataminute), tip(tip) {}

string Film::getTitlu() const {
    return titlu;
}

int Film::getDurataMinute() const {
    return durataminute;
}

TipFilm Film::getTip() const {
    return tip;
}

void Film::afisare() const {
    std::cout << "Titlu: " << titlu << std::endl;
    std::cout << "Durata: " << durataminute << " minute" << std::endl;
    std::cout << "Tip: " << (tip == TipFilm::Film2D ? "2D" : "3D") << std::endl;
}