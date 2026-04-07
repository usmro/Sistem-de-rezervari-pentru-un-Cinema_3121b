#include <string>
#include <iostream>

using std::string;

enum class TipFilm{
    Film2D, 
    Film3D
};

class Film {
private:
    string titlu;
    int durataminute;
    TipFilm tip;
public:
    Film(string titlu, int durataminute, TipFilm tip);

    std::string getTitlu() const;
    int getDurataMinute() const;
    TipFilm getTip() const;

    void afisare() const;
};