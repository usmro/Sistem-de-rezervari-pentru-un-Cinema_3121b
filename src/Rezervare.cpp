#include "Rezervare.h"
#include "Exceptii.h"
#include <fstream>
#include <iomanip>
#include <sstream>

Rezervare::Rezervare(const std::string &idRezervare,
                     std::shared_ptr<Proiectie> proiectie, int rand, int loc,
                     const std::string &tipBilet, double pretFinal,
                     const std::string &usernameClient)
    : idRezervare(idRezervare), proiectie(std::move(proiectie)), rand(rand),
      loc(loc), tipBilet(tipBilet), pretFinal(pretFinal),
      usernameClient(usernameClient), anulata(false) {}
const std::string &Rezervare::getId() const { return idRezervare; }
std::shared_ptr<Proiectie> Rezervare::getProiectie() const { return proiectie; }
int Rezervare::getRand() const { return rand; }
int Rezervare::getLoc() const { return loc; }
const std::string &Rezervare::getTipBilet() const { return tipBilet; }
double Rezervare::getPretFinal() const { return pretFinal; }
const std::string &Rezervare::getUsernameClient() const {
  return usernameClient;
}
bool Rezervare::esteAnulata() const { return anulata; }
void Rezervare::anuleaza() { anulata = true; }
void Rezervare::genereazaBiletTxt() const {
  std::string numeFisier = "data/" + idRezervare + "_bilet.txt";
  std::ofstream fout(numeFisier);
  if (!fout.is_open()) {
    throw FisierInvalidExceptie(numeFisier);
  }
  fout << "============================================\n";
  fout << "          BILET CINEMA - CONFIRMARE         \n";
  fout << "============================================\n";
  fout << "  ID Rezervare: " << idRezervare << "\n";
  fout << "--------------------------------------------\n";
  fout << "  Film:     " << proiectie->getFilm()->getTitlu() << " ("
       << proiectie->getFilm()->tipToString() << ")\n";
  fout << "  Gen:      " << proiectie->getFilm()->getGen() << "\n";
  fout << "  Durata:   " << proiectie->getFilm()->getDurataMinute() << " min\n";
  fout << "  Sala:     " << proiectie->getSala()->getNume() << "\n";
  fout << "  Data/Ora: " << proiectie->getDataOraString() << "\n";
  fout << "--------------------------------------------\n";
  fout << "  Loc:      Randul " << rand << ", Locul " << loc << "\n";
  fout << "  Tip:      " << tipBilet << "\n";
  fout << std::fixed << std::setprecision(2);
  fout << "  Pret:     " << pretFinal << " RON\n";
  fout << "--------------------------------------------\n";
  fout << "  Client:   " << usernameClient << "\n";
  if (anulata) {
    fout << "  STATUS:   *** ANULAT ***\n";
  }
  fout << "============================================\n";
  fout << "       Va multumim! Vizionare placuta!      \n";
  fout << "============================================\n";
  fout.close();
}
std::string Rezervare::toCSV() const {
  return idRezervare + "," + std::to_string(proiectie->getId()) + "," +
         std::to_string(rand) + "," + std::to_string(loc) + "," + tipBilet +
         "," + std::to_string(pretFinal) + "," + usernameClient + "," +
         (anulata ? "1" : "0");
}
