#pragma once
#include <string>
enum class TipFilm { Film2D, Film3D };
class Film {
private:
  std::string titlu;
  int durataminute;
  TipFilm tip;
  std::string gen;
  double pretBaza;

public:
  Film(const std::string &titlu, int durataminute, TipFilm tip,
       const std::string &gen, double pretBaza);
  const std::string &getTitlu() const;
  int getDurataMinute() const;
  TipFilm getTip() const;
  const std::string &getGen() const;
  double getPretBaza() const;
  std::string tipToString() const;
  std::string toCSV() const;
  static Film fromCSV(const std::string &linie);
};
