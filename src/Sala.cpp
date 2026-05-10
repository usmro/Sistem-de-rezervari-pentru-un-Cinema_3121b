#include "Sala.h"
#include <sstream>
#include <stdexcept>
void Sala::aplicaConfiguratieTip() {
  switch (tipSala) {
  case TipSala::Standard:
    numarRanduri = 10;
    locuriPeRand = 15;
    multiplicatorPret = 1.00;
    break;
  case TipSala::IMAX:
    numarRanduri = 8;
    locuriPeRand = 12;
    multiplicatorPret = 1.50;
    break;
  case TipSala::VIP:
    numarRanduri = 4;
    locuriPeRand = 6;
    multiplicatorPret = 2.00;
    break;
  }
  matriceLocuri = std::vector<std::vector<bool>>(
      numarRanduri, std::vector<bool>(locuriPeRand, true));
}
Sala::Sala(const std::string &numeSala, TipSala tipSala)
    : numeSala(numeSala), tipSala(tipSala), numarRanduri(0), locuriPeRand(0),
      multiplicatorPret(1.0) {
  aplicaConfiguratieTip();
}
Sala::Sala(const std::string &numeSala, TipSala tipSala, int numarRanduri,
           int locuriPeRand)
    : numeSala(numeSala), tipSala(tipSala), numarRanduri(numarRanduri),
      locuriPeRand(locuriPeRand) {
  switch (tipSala) {
  case TipSala::Standard:
    multiplicatorPret = 1.00;
    break;
  case TipSala::IMAX:
    multiplicatorPret = 1.50;
    break;
  case TipSala::VIP:
    multiplicatorPret = 2.00;
    break;
  }
  matriceLocuri = std::vector<std::vector<bool>>(
      numarRanduri, std::vector<bool>(locuriPeRand, true));
}
const std::string &Sala::getNume() const { return numeSala; }
TipSala Sala::getTipSala() const { return tipSala; }
int Sala::getNumarRanduri() const { return numarRanduri; }
int Sala::getLocuriPeRand() const { return locuriPeRand; }
double Sala::getMultiplicatorPret() const { return multiplicatorPret; }
int Sala::getCapacitateTotala() const { return numarRanduri * locuriPeRand; }
std::string Sala::tipToString() const {
  switch (tipSala) {
  case TipSala::Standard:
    return "Standard";
  case TipSala::IMAX:
    return "IMAX";
  case TipSala::VIP:
    return "VIP";
  }
  return "Standard";
}
TipSala Sala::tipFromString(const std::string &s) {
  if (s == "IMAX")
    return TipSala::IMAX;
  if (s == "VIP")
    return TipSala::VIP;
  return TipSala::Standard;
}
bool Sala::esteLocDisponibil(int rand, int loc) const {
  if (rand < 1 || rand > numarRanduri || loc < 1 || loc > locuriPeRand) {
    throw IndexInvalidExceptie();
  }
  return matriceLocuri[rand - 1][loc - 1];
}
const std::vector<std::vector<bool>> &Sala::getMatriceLocuri() const {
  return matriceLocuri;
}
void Sala::rezervaLoc(int rand, int loc) {
  if (rand < 1 || rand > numarRanduri || loc < 1 || loc > locuriPeRand) {
    throw IndexInvalidExceptie();
  }
  if (!matriceLocuri[rand - 1][loc - 1]) {
    throw LocOcupatExceptie();
  }
  matriceLocuri[rand - 1][loc - 1] = false;
}
void Sala::elibereazaLoc(int rand, int loc) {
  if (rand < 1 || rand > numarRanduri || loc < 1 || loc > locuriPeRand) {
    throw IndexInvalidExceptie();
  }
  if (matriceLocuri[rand - 1][loc - 1]) {
    throw LocLiber();
  }
  matriceLocuri[rand - 1][loc - 1] = true;
}
void Sala::reseteazaLocuri() {
  for (int i = 0; i < numarRanduri; ++i)
    for (int j = 0; j < locuriPeRand; ++j)
      matriceLocuri[i][j] = true;
}
std::string Sala::toCSV() const {
  return numeSala + "," + tipToString() + "," + std::to_string(numarRanduri) +
         "," + std::to_string(locuriPeRand);
}
Sala Sala::fromCSV(const std::string &linie) {
  std::istringstream ss(linie);
  std::string nume, tipStr, randuriStr, locuriStr;
  std::getline(ss, nume, ',');
  std::getline(ss, tipStr, ',');
  std::getline(ss, randuriStr, ',');
  std::getline(ss, locuriStr, ',');
  TipSala tip = tipFromString(tipStr);
  return Sala(nume, tip, std::stoi(randuriStr), std::stoi(locuriStr));
}
