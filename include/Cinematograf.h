#pragma once
#include "BiletFactory.h"
#include "Film.h"
#include "ProdusConsumabil.h"
#include "Proiectie.h"
#include "Rezervare.h"
#include "Sala.h"
#include "User.h"
#include "Voucher.h"
#include <memory>
#include <string>
#include <vector>
class Cinematograf {
private:
  std::string nume;
  std::vector<std::shared_ptr<Film>> filme;
  std::vector<std::shared_ptr<Sala>> sali;
  std::vector<std::shared_ptr<Proiectie>> proiectii;
  std::vector<Rezervare> rezervari;
  std::vector<std::shared_ptr<User>> useri;
  std::vector<Voucher> vouchere;
  std::vector<ProdusConsumabil> produse;
  struct VIPExtraOrder {
    std::string idRezervare;
    std::string extras; // JSON array string
  };
  std::vector<VIPExtraOrder> vipExtras;
  int nextIdProiectie;
  int nextIdRezervare;
  Cinematograf();
  Cinematograf(const Cinematograf &) = delete;
  Cinematograf &operator=(const Cinematograf &) = delete;
  Cinematograf(Cinematograf &&) = delete;
  Cinematograf &operator=(Cinematograf &&) = delete;

public:
  ProdusConsumabil &getProdus(int index);
  std::vector<std::shared_ptr<Proiectie>> getProiectiiVIP() const;
  std::vector<std::shared_ptr<Proiectie>>
  getProiectiiPentruFilm(const std::string &titluFilm) const;
  static Cinematograf &getInstance();
  void setNume(const std::string &nume);
  const std::string &getNume() const;
  void adaugaProdus(const ProdusConsumabil &produs);
  const std::vector<ProdusConsumabil> &getProduse() const;
  void adaugaFilm(std::shared_ptr<Film> film);
  const std::vector<std::shared_ptr<Film>> &getFilme() const;
  std::shared_ptr<Film> gasesteFilm(const std::string &titlu) const;
  void adaugaSala(std::shared_ptr<Sala> sala);
  const std::vector<std::shared_ptr<Sala>> &getSali() const;
  std::shared_ptr<Sala> gasesteSala(const std::string &nume) const;
  void adaugaProiectie(std::shared_ptr<Film> film, std::shared_ptr<Sala> sala,
                       const std::tm &dataOra,
                       FormatAudio formatAudio = FormatAudio::Subtitrat);
  const std::vector<std::shared_ptr<Proiectie>> &getProiectii() const;
  std::vector<std::shared_ptr<Proiectie>> getProiectiiViitoare() const;
  Rezervare &creeazaRezervare(std::shared_ptr<Proiectie> proiectie, int rand,
                              int loc, const std::string &tipBilet,
                              const std::string &username,
                              double pretVoucher = -1.0);
  void anuleazaRezervare(const std::string &idRezervare,
                         const std::string &username, RolUser rol);
  const std::vector<Rezervare> &getRezervari() const;
  std::vector<Rezervare> getRezervariUser(const std::string &username) const;
  void adaugaUser(std::shared_ptr<User> user);
  User *autentifica(const std::string &username, const std::string &parola);
  bool existaUser(const std::string &username) const;
  const std::vector<std::shared_ptr<User>> &getUseri() const;
  User *gasesteUser(const std::string &username);
  void stergeFilm(const std::string &titlu);
  void adaugaVoucher(const Voucher &voucher);
  void stergeVoucher(const std::string &cod);
  Voucher *gasesteVoucher(const std::string &cod);
  const std::vector<Voucher> &getVouchere() const;
  bool existaVoucher(const std::string &cod) const;
  void setNextIdProiectie(int id);
  void setNextIdRezervare(int id);
  void setRezervari(const std::vector<Rezervare> &rez);
  void addProiectieDirect(std::shared_ptr<Proiectie> p);
  User *autentificaAdmin(const std::string &username,
                         const std::string &parola);
  User *autentificaClient(const std::string &username,
                          const std::string &parola);
  void adaugaVIPExtras(const std::string &idRezervare,
                       const std::string &extrasJson);
  const std::vector<VIPExtraOrder> &getVIPExtras() const;
};
