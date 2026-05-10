#include "Film.h"
#include <sstream>
#include <stdexcept>
Film::Film(const std::string& titlu, int durataminute, TipFilm tip,
           const std::string& gen, double pretBaza)
    : titlu(titlu), durataminute(durataminute), tip(tip), gen(gen), pretBaza(pretBaza) {}
const std::string& Film::getTitlu() const { return titlu; }
int Film::getDurataMinute() const { return durataminute; }
TipFilm Film::getTip() const { return tip; }
const std::string& Film::getGen() const { return gen; }
double Film::getPretBaza() const { return pretBaza; }
std::string Film::tipToString() const {
    return (tip == TipFilm::Film2D) ? "2D" : "3D";
}
std::string Film::toCSV() const {
    return titlu + "," + std::to_string(durataminute) + "," +
           tipToString() + "," + gen + "," + std::to_string(pretBaza);
}
Film Film::fromCSV(const std::string& linie) {
    std::istringstream ss(linie);
    std::string titlu, durataStr, tipStr, gen, pretStr;
    std::getline(ss, titlu, ',');
    std::getline(ss, durataStr, ',');
    std::getline(ss, tipStr, ',');
    std::getline(ss, gen, ',');
    std::getline(ss, pretStr, ',');
    int durata = std::stoi(durataStr);
    TipFilm tip = (tipStr == "3D") ? TipFilm::Film3D : TipFilm::Film2D;
    double pret = std::stod(pretStr);
    return Film(titlu, durata, tip, gen, pret);
}
