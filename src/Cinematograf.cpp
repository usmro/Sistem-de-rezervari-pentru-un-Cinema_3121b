#include "Cinematograf.h"
#include "Exceptii.h"
#include <algorithm>
#include <sstream>
ProdusConsumabil &Cinematograf::getProdus(int index) {
  return produse.at(index);
}
Cinematograf::Cinematograf()
    : nume("Cinema City"), nextIdProiectie(1), nextIdRezervare(1) {}
Cinematograf &Cinematograf::getInstance() {
  static Cinematograf instance;
  return instance;
}
void Cinematograf::setNume(const std::string &n) { nume = n; }
const std::string &Cinematograf::getNume() const { return nume; }
void Cinematograf::adaugaFilm(std::shared_ptr<Film> film) {
  filme.push_back(std::move(film));
}
const std::vector<std::shared_ptr<Film>> &Cinematograf::getFilme() const {
  return filme;
}
std::shared_ptr<Film>
Cinematograf::gasesteFilm(const std::string &titlu) const {
  for (const auto &f : filme) {
    if (f->getTitlu() == titlu)
      return f;
  }
  return nullptr;
}
void Cinematograf::adaugaSala(std::shared_ptr<Sala> sala) {
  sali.push_back(std::move(sala));
}
const std::vector<std::shared_ptr<Sala>> &Cinematograf::getSali() const {
  return sali;
}
std::shared_ptr<Sala>
Cinematograf::gasesteSala(const std::string &numeSala) const {
  for (const auto &s : sali) {
    if (s->getNume() == numeSala)
      return s;
  }
  return nullptr;
}
void Cinematograf::adaugaProiectie(std::shared_ptr<Film> film,
                                   std::shared_ptr<Sala> sala,
                                   const std::tm &dataOra,
                                   FormatAudio formatAudio) {
  std::tm copie = dataOra;
  std::time_t timpProiectie = std::mktime(&copie);
  std::time_t acum = std::time(nullptr);
  if (std::difftime(timpProiectie, acum) < 0)
    throw DataInTrecutExceptie();
  auto proiectie = std::make_shared<Proiectie>(nextIdProiectie++, film, sala,
                                               dataOra, formatAudio);
  proiectii.push_back(proiectie);
}
const std::vector<std::shared_ptr<Proiectie>> &
Cinematograf::getProiectii() const {
  return proiectii;
}
std::vector<std::shared_ptr<Proiectie>>
Cinematograf::getProiectiiViitoare() const {
  std::vector<std::shared_ptr<Proiectie>> viitoare;
  for (const auto &p : proiectii) {
    if (!p->aInceput()) {
      viitoare.push_back(p);
    }
  }
  return viitoare;
}
Rezervare &Cinematograf::creeazaRezervare(std::shared_ptr<Proiectie> proiectie,
                                          int rand, int loc,
                                          const std::string &tipBilet,
                                          const std::string &username,
                                          double pretVoucher) {
  if (proiectie->aInceput()) {
    throw AnulareImposibilaExceptie();
  }
  proiectie->getSala()->rezervaLoc(rand, loc);
  double pretFinal;
  if (pretVoucher > 0.0) {
    pretFinal = pretVoucher;
  } else {
    double multSala = proiectie->getSala()->getMultiplicatorPret();
    pretFinal = BiletFactory::calculeazaPret(
        tipBilet, proiectie->getFilm()->getPretBaza(), multSala);
  }
  User *client = gasesteUser(username);
  if (client && client->getAreCardFidelitate()) {
    pretFinal *= 0.90;
  }
  std::string idRez = "REZ" + std::to_string(nextIdRezervare++);
  rezervari.emplace_back(idRez, proiectie, rand, loc, tipBilet, pretFinal,
                         username);
  rezervari.back().genereazaBiletTxt();
  return rezervari.back();
}
void Cinematograf::anuleazaRezervare(const std::string &idRezervare,
                                     const std::string &username, RolUser rol) {
  for (auto &rez : rezervari) {
    if (rez.getId() == idRezervare && !rez.esteAnulata()) {
      if (rol == RolUser::Client && rez.getUsernameClient() != username) {
        throw std::runtime_error(
            "Nu puteti anula rezervarea altui utilizator!");
      }
      if (rez.getProiectie()->aInceput()) {
        throw AnulareImposibilaExceptie();
      }
      rez.getProiectie()->getSala()->elibereazaLoc(rez.getRand(), rez.getLoc());
      rez.anuleaza();
      return;
    }
  }
  throw RezervareInexistentaExceptie();
}
const std::vector<Rezervare> &Cinematograf::getRezervari() const {
  return rezervari;
}
std::vector<Rezervare>
Cinematograf::getRezervariUser(const std::string &username) const {
  std::vector<Rezervare> rezultat;
  for (const auto &r : rezervari) {
    if (r.getUsernameClient() == username) {
      rezultat.push_back(r);
    }
  }
  return rezultat;
}
void Cinematograf::adaugaUser(std::shared_ptr<User> user) {
  if (existaUser(user->getUsername()))
    throw UsernameExistaExceptie();
  useri.push_back(std::move(user));
}
User *Cinematograf::autentifica(const std::string &username,
                                const std::string &parola) {
  for (auto &u : useri) {
    if (u->getUsername() == username && u->verificaParola(parola))
      return u.get();
  }
  throw AutentificareEsuataExceptie();
}
bool Cinematograf::existaUser(const std::string &username) const {
  for (const auto &u : useri)
    if (u->getUsername() == username)
      return true;
  return false;
}
User *Cinematograf::gasesteUser(const std::string &username) {
  for (auto &u : useri)
    if (u->getUsername() == username)
      return u.get();
  return nullptr;
}
const std::vector<std::shared_ptr<User>> &Cinematograf::getUseri() const {
  return useri;
}
void Cinematograf::adaugaVoucher(const Voucher &voucher) {
  vouchere.push_back(voucher);
}
Voucher *Cinematograf::gasesteVoucher(const std::string &cod) {
  for (auto &v : vouchere) {
    if (v.getCod() == cod)
      return &v;
  }
  return nullptr;
}
const std::vector<Voucher> &Cinematograf::getVouchere() const {
  return vouchere;
}
bool Cinematograf::existaVoucher(const std::string &cod) const {
  for (const auto &v : vouchere) {
    if (v.getCod() == cod)
      return true;
  }
  return false;
}
void Cinematograf::setNextIdProiectie(int id) { nextIdProiectie = id; }
void Cinematograf::setNextIdRezervare(int id) { nextIdRezervare = id; }
void Cinematograf::setRezervari(const std::vector<Rezervare> &rez) {
  rezervari = rez;
}
void Cinematograf::addProiectieDirect(std::shared_ptr<Proiectie> p) {
  proiectii.push_back(std::move(p));
}
void Cinematograf::adaugaProdus(const ProdusConsumabil &produs) {
  produse.push_back(produs);
}
const std::vector<ProdusConsumabil> &Cinematograf::getProduse() const {
  return produse;
}
std::vector<std::shared_ptr<Proiectie>> Cinematograf::getProiectiiVIP() const {
  std::vector<std::shared_ptr<Proiectie>> rezultat;
  for (const auto &p : proiectii) {
    if (p->getSala()->getTipSala() == TipSala::VIP)
      rezultat.push_back(p);
  }
  return rezultat;
}
std::vector<std::shared_ptr<Proiectie>>
Cinematograf::getProiectiiPentruFilm(const std::string &titluFilm) const {
  std::vector<std::shared_ptr<Proiectie>> rezultat;
  for (const auto &p : proiectii) {
    if (!p->aInceput() && p->getFilm()->getTitlu() == titluFilm)
      rezultat.push_back(p);
  }
  return rezultat;
}
User *Cinematograf::autentificaAdmin(const std::string &username,
                                     const std::string &parola) {
  for (auto &u : useri) {
    if (u->getUsername() == username && u->verificaParola(parola) &&
        u->getRol() == RolUser::Admin)
      return u.get();
  }
  throw AutentificareEsuataExceptie();
}
User *Cinematograf::autentificaClient(const std::string &username,
                                      const std::string &parola) {
  for (auto &u : useri) {
    if (u->getUsername() == username && u->verificaParola(parola) &&
        u->getRol() == RolUser::Client)
      return u.get();
  }
  throw AutentificareEsuataExceptie();
}
