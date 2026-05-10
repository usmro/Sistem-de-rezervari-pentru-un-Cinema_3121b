#pragma once
#include "Cinematograf.h"
#include "StorageService.h"
#include "User.h"
#include <windows.h>
class ConsoleUI {
private:
  void clearScreen() const;
  void pauseScreen() const;
  int citesteOptiune(int minim, int maxim) const;
  std::string citesteString(const std::string &prompt) const;
  void meniuAutentificare();
  void meniuInregistrare();
  void meniuAdmin(User &user);
  void meniuClient(User &user);
  void adminAdaugaFilm();
  void adminAdaugaSala();
  void adminAdaugaProiectie();
  void adminVeziRezervari();
  void adminAfisareHartaLocuri();
  void adminAnuleazaRezervare();
  void adminGestionareVouchere();
  void clientVeziProgram();
  void clientRezervaLoc(User &user);
  void clientIstoricRezervari(const User &user);
  void clientAnuleazaRezervare(const User &user);
  void afisareFilm(const Film &film) const;
  void afisareHartaLocuri(const Sala &sala) const;
  void afisareProiectie(const Proiectie &proiectie, int index) const;
  void afisareRezervare(const Rezervare &rez) const;
  void afisareVoucher(const Voucher &v) const;
  void afisareHeader() const;
  void adminGestionareSnacks();
  void adminPOSBar();
  void clientPrecomandaSnacks(const User &user);
public:
  void ruleaza();
};
