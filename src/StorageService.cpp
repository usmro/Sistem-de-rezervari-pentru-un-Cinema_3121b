#include "StorageService.h"
#include "Angajat.h"
#include "Exceptii.h"
#include "FormatAudio.h"
#include <fstream>
#include <iostream>
#include <sstream>
#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif
const std::string StorageService::DIRECTOR_DATE = "data";
const std::string StorageService::FISIER_FILME = "data/filme.csv";
const std::string StorageService::FISIER_SALI = "data/sali.csv";
const std::string StorageService::FISIER_PROIECTII = "data/proiectii.csv";
const std::string StorageService::FISIER_REZERVARI = "data/rezervari.csv";
const std::string StorageService::FISIER_USERI = "data/useri.csv";
const std::string StorageService::FISIER_VOUCHERE = "data/vouchere.csv";
void StorageService::asiguraDirector() { MKDIR(DIRECTOR_DATE.c_str()); }
void StorageService::salveazaFilme(
    const std::vector<std::shared_ptr<Film>> &filme) {
  asiguraDirector();
  std::ofstream fout(FISIER_FILME);
  if (!fout.is_open())
    throw FisierInvalidExceptie(FISIER_FILME);
  for (const auto &f : filme) {
    fout << f->toCSV() << "\n";
  }
}
std::vector<std::shared_ptr<Film>> StorageService::incarcaFilme() {
  std::vector<std::shared_ptr<Film>> filme;
  std::ifstream fin(FISIER_FILME);
  if (!fin.is_open())
    return filme;
  std::string linie;
  while (std::getline(fin, linie)) {
    if (linie.empty())
      continue;
    try {
      filme.push_back(std::make_shared<Film>(Film::fromCSV(linie)));
    } catch (const std::exception &e) {
      std::cerr << "[WARN] Linie invalida in filme.csv: " << e.what() << "\n";
    }
  }
  return filme;
}
void StorageService::salveazaSali(
    const std::vector<std::shared_ptr<Sala>> &sali) {
  asiguraDirector();
  std::ofstream fout(FISIER_SALI);
  if (!fout.is_open())
    throw FisierInvalidExceptie(FISIER_SALI);
  for (const auto &s : sali) {
    fout << s->toCSV() << "\n";
  }
}
std::vector<std::shared_ptr<Sala>> StorageService::incarcaSali() {
  std::vector<std::shared_ptr<Sala>> sali;
  std::ifstream fin(FISIER_SALI);
  if (!fin.is_open())
    return sali;
  std::string linie;
  while (std::getline(fin, linie)) {
    if (linie.empty())
      continue;
    try {
      sali.push_back(std::make_shared<Sala>(Sala::fromCSV(linie)));
    } catch (const std::exception &e) {
      std::cerr << "[WARN] Linie invalida in sali.csv: " << e.what() << "\n";
    }
  }
  return sali;
}
void StorageService::salveazaProiectii(
    const std::vector<std::shared_ptr<Proiectie>> &proiectii) {
  asiguraDirector();
  std::ofstream fout(FISIER_PROIECTII);
  if (!fout.is_open())
    throw FisierInvalidExceptie(FISIER_PROIECTII);
  for (const auto &p : proiectii) {
    fout << p->toCSV() << "\n";
  }
}
std::vector<std::shared_ptr<Proiectie>> StorageService::incarcaProiectii(
    const std::vector<std::shared_ptr<Film>> &filme,
    const std::vector<std::shared_ptr<Sala>> &sali) {
  std::vector<std::shared_ptr<Proiectie>> proiectii;
  std::ifstream fin(FISIER_PROIECTII);
  if (!fin.is_open())
    return proiectii;
  std::string linie;
  while (std::getline(fin, linie)) {
    if (linie.empty())
      continue;
    try {
      std::istringstream ss(linie);
      std::string idStr, titluFilm, numeSala, dataOraStr, formatStr;
      std::getline(ss, idStr, ',');
      std::getline(ss, titluFilm, ',');
      std::getline(ss, numeSala, ',');
      std::getline(ss, dataOraStr, ',');
      std::getline(ss, formatStr);
      int id = std::stoi(idStr);
      FormatAudio format = formatStr.empty() ? FormatAudio::Subtitrat
                                             : formatAudioFromString(formatStr);
      std::shared_ptr<Film> film = nullptr;
      for (const auto &f : filme)
        if (f->getTitlu() == titluFilm) {
          film = f;
          break;
        }
      std::shared_ptr<Sala> sala = nullptr;
      for (const auto &s : sali)
        if (s->getNume() == numeSala) {
          sala = s;
          break;
        }
      if (film && sala) {
        auto dt = Proiectie::parseDataOra(dataOraStr);
        proiectii.push_back(
            std::make_shared<Proiectie>(id, film, sala, dt, format));
      }
    } catch (const std::exception &e) {
      std::cerr << "[WARN] Linie invalida in proiectii.csv: " << e.what()
                << "\n";
    }
  }
  return proiectii;
}
void StorageService::salveazaRezervari(
    const std::vector<Rezervare> &rezervari) {
  asiguraDirector();
  std::ofstream fout(FISIER_REZERVARI);
  if (!fout.is_open())
    throw FisierInvalidExceptie(FISIER_REZERVARI);
  for (const auto &r : rezervari) {
    fout << r.toCSV() << "\n";
  }
}
std::vector<Rezervare> StorageService::incarcaRezervari(
    const std::vector<std::shared_ptr<Proiectie>> &proiectii) {
  std::vector<Rezervare> rezervari;
  std::ifstream fin(FISIER_REZERVARI);
  if (!fin.is_open())
    return rezervari;
  std::string linie;
  while (std::getline(fin, linie)) {
    if (linie.empty())
      continue;
    try {
      std::istringstream ss(linie);
      std::string idRez, idProiStr, randStr, locStr, tipStr, pretStr, username,
          anulatStr;
      std::getline(ss, idRez, ',');
      std::getline(ss, idProiStr, ',');
      std::getline(ss, randStr, ',');
      std::getline(ss, locStr, ',');
      std::getline(ss, tipStr, ',');
      std::getline(ss, pretStr, ',');
      std::getline(ss, username, ',');
      std::getline(ss, anulatStr, ',');
      int idProi = std::stoi(idProiStr);
      std::shared_ptr<Proiectie> proiectie = nullptr;
      for (const auto &p : proiectii) {
        if (p->getId() == idProi) {
          proiectie = p;
          break;
        }
      }
      if (proiectie) {
        double pret = std::stod(pretStr);
        Rezervare rez(idRez, proiectie, std::stoi(randStr), std::stoi(locStr),
                      tipStr, pret, username);
        if (anulatStr == "1") {
          rez.anuleaza();
        } else {
          try {
            proiectie->getSala()->rezervaLoc(std::stoi(randStr),
                                             std::stoi(locStr));
          } catch (...) {
          }
        }
        rezervari.push_back(rez);
      }
    } catch (const std::exception &e) {
      std::cerr << "[WARN] Linie invalida in rezervari.csv: " << e.what()
                << "\n";
    }
  }
  return rezervari;
}
void StorageService::salveazaUseri(
    const std::vector<std::shared_ptr<User>> &useri) {
  asiguraDirector();
  std::ofstream fout(FISIER_USERI);
  if (!fout.is_open())
    throw FisierInvalidExceptie(FISIER_USERI);
  for (const auto &u : useri)
    fout << u->toCSV() << "\n";
}
std::vector<std::shared_ptr<User>>
StorageService::incarcaUseri(const std::vector<std::shared_ptr<Sala>> &sali) {
  std::vector<std::shared_ptr<User>> useri;
  std::ifstream fin(FISIER_USERI);
  if (!fin.is_open())
    return useri;
  std::string linie;
  while (std::getline(fin, linie)) {
    if (linie.empty())
      continue;
    try {
      std::istringstream ss(linie);
      std::string user, pass, rolStr;
      std::getline(ss, user, ',');
      std::getline(ss, pass, ',');
      std::getline(ss, rolStr, ',');
      if (rolStr == "Angajat") {
        std::string program, numeSala;
        std::getline(ss, program, ',');
        std::getline(ss, numeSala, ',');
        std::shared_ptr<Sala> sala = nullptr;
        for (const auto &s : sali)
          if (s->getNume() == numeSala) {
            sala = s;
            break;
          }
        useri.push_back(std::make_shared<Angajat>(user, pass, program, sala));
      } else {
        std::string cardStr;
        std::getline(ss, cardStr, ',');
        RolUser rol = (rolStr == "Admin") ? RolUser::Admin : RolUser::Client;
        bool card = (cardStr == "1");
        useri.push_back(std::make_shared<User>(user, pass, rol, card));
      }
    } catch (const std::exception &e) {
      std::cerr << "[WARN] Linie invalida in useri.csv: " << e.what() << "\n";
    }
  }
  return useri;
}
void StorageService::salveazaVouchere(const std::vector<Voucher> &vouchere) {
  asiguraDirector();
  std::ofstream fout(FISIER_VOUCHERE);
  if (!fout.is_open())
    throw FisierInvalidExceptie(FISIER_VOUCHERE);
  for (const auto &v : vouchere) {
    fout << v.toCSV() << "\n";
  }
}
std::vector<Voucher> StorageService::incarcaVouchere() {
  std::vector<Voucher> vouchere;
  std::ifstream fin(FISIER_VOUCHERE);
  if (!fin.is_open())
    return vouchere;
  std::string linie;
  while (std::getline(fin, linie)) {
    if (linie.empty())
      continue;
    try {
      vouchere.push_back(Voucher::fromCSV(linie));
    } catch (const std::exception &e) {
      std::cerr << "[WARN] Linie invalida in vouchere.csv: " << e.what()
                << "\n";
    }
  }
  return vouchere;
}
const std::string StorageService::FISIER_SNACKS = "data/snacks.csv";
void StorageService::salveazaSnacks(
    const std::vector<ProdusConsumabil> &produse) {
  asiguraDirector();
  std::ofstream fout(FISIER_SNACKS);
  if (!fout.is_open())
    throw FisierInvalidExceptie(FISIER_SNACKS);
  for (const auto &p : produse)
    fout << p.toCSV() << "\n";
}
std::vector<ProdusConsumabil> StorageService::incarcaSnacks() {
  std::vector<ProdusConsumabil> produse;
  std::ifstream fin(FISIER_SNACKS);
  if (!fin.is_open())
    return produse;
  std::string linie;
  while (std::getline(fin, linie)) {
    if (linie.empty())
      continue;
    try {
      produse.push_back(ProdusConsumabil::fromCSV(linie));
    } catch (const std::exception &e) {
      std::cerr << "[WARN] Linie invalida in snacks.csv: " << e.what() << "\n";
    }
  }
  return produse;
}
