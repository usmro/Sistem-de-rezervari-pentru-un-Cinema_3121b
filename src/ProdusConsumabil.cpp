#include "ProdusConsumabil.h"
#include <sstream>
ProdusConsumabil::ProdusConsumabil(const std::string &nume,
                                   const std::string &descriere, double pret,
                                   int stoc)
    : nume(nume), descriere(descriere), pret(pret), stoc(stoc) {}
const std::string &ProdusConsumabil::getNume() const { return nume; }
const std::string &ProdusConsumabil::getDescriere() const { return descriere; }
double ProdusConsumabil::getPret() const { return pret; }
int ProdusConsumabil::getStoc() const { return stoc; }
void ProdusConsumabil::setStoc(int s) { stoc = s; }
bool ProdusConsumabil::esteDisponibil() const { return stoc > 0; }
std::string ProdusConsumabil::toCSV() const {
  return nume + "," + descriere + "," + std::to_string(pret) + "," +
         std::to_string(stoc);
}
ProdusConsumabil ProdusConsumabil::fromCSV(const std::string &linie) {
  std::istringstream ss(linie);
  std::string numeS, descS, pretS, stocS;
  std::getline(ss, numeS, ',');
  std::getline(ss, descS, ',');
  std::getline(ss, pretS, ',');
  std::getline(ss, stocS, ',');
  return ProdusConsumabil(numeS, descS, std::stod(pretS), std::stoi(stocS));
}
