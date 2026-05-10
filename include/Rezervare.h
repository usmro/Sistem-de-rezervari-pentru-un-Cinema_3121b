#pragma once
#include "Proiectie.h"
#include <memory>
#include <string>

class Rezervare {
private:
  std::string idRezervare;
  std::shared_ptr<Proiectie> proiectie;
  int rand;
  int loc;
  std::string tipBilet;
  double pretFinal;
  std::string usernameClient;
  bool anulata;

public:
  Rezervare(const std::string &idRezervare,
            std::shared_ptr<Proiectie> proiectie, int rand, int loc,
            const std::string &tipBilet, double pretFinal,
            const std::string &usernameClient);
  const std::string &getId() const;
  std::shared_ptr<Proiectie> getProiectie() const;
  int getRand() const;
  int getLoc() const;
  const std::string &getTipBilet() const;
  double getPretFinal() const;
  const std::string &getUsernameClient() const;
  bool esteAnulata() const;
  void anuleaza();
  void genereazaBiletTxt() const;
  std::string toCSV() const;
};
