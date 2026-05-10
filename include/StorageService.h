#pragma once
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
class StorageService {
private:
  static const std::string DIRECTOR_DATE;
  static const std::string FISIER_FILME;
  static const std::string FISIER_SALI;
  static const std::string FISIER_PROIECTII;
  static const std::string FISIER_REZERVARI;
  static const std::string FISIER_USERI;
  static const std::string FISIER_VOUCHERE;
  static const std::string FISIER_SNACKS;
  static void asiguraDirector();

public:
  static void salveazaSnacks(const std::vector<ProdusConsumabil> &produse);
  static std::vector<ProdusConsumabil> incarcaSnacks();
  static void salveazaFilme(const std::vector<std::shared_ptr<Film>> &filme);
  static std::vector<std::shared_ptr<Film>> incarcaFilme();
  static void salveazaSali(const std::vector<std::shared_ptr<Sala>> &sali);
  static std::vector<std::shared_ptr<Sala>> incarcaSali();
  static void
  salveazaProiectii(const std::vector<std::shared_ptr<Proiectie>> &proiectii);
  static std::vector<std::shared_ptr<Proiectie>>
  incarcaProiectii(const std::vector<std::shared_ptr<Film>> &filme,
                   const std::vector<std::shared_ptr<Sala>> &sali);
  static void salveazaRezervari(const std::vector<Rezervare> &rezervari);
  static std::vector<Rezervare>
  incarcaRezervari(const std::vector<std::shared_ptr<Proiectie>> &proiectii);
  static void salveazaUseri(const std::vector<std::shared_ptr<User>> &useri);
  static std::vector<std::shared_ptr<User>>
  incarcaUseri(const std::vector<std::shared_ptr<Sala>> &sali);
  static void salveazaVouchere(const std::vector<Voucher> &vouchere);
  static std::vector<Voucher> incarcaVouchere();
};
