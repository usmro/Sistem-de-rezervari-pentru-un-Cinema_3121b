#include "ConsoleUI.h"
#include "Exceptii.h"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::string;
void ConsoleUI::clearScreen() const {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}
void ConsoleUI::pauseScreen() const {
  cout << "\nApasati ENTER pentru a continua...";
  cin.get();
}
int ConsoleUI::citesteOptiune(int minim, int maxim) const {
  int optiune;
  while (true) {
    cout << "Alegeti o optiune (" << minim << "-" << maxim << "): ";
    if (cin >> optiune && optiune >= minim && optiune <= maxim) {
      cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      return optiune;
    }
    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cout << "[!] Optiune invalida. Incercati din nou.\n";
  }
}
string ConsoleUI::citesteString(const string &prompt) const {
  string valoare;
  cout << prompt;
  std::getline(cin, valoare);
  return valoare;
}
void ConsoleUI::afisareHeader() const {
  auto &cinema = Cinematograf::getInstance();
  cout << "\n";
  cout << "========================================================\n";
  cout << "   ____  _                                              \n";
  cout << "  / ___|(_)_ __   ___ _ __ ___   __ _                  \n";
  cout << " | |    | | '_ \\ / _ \\ '_ ` _ \\ / _` |             \n";
  cout << " | |___ | | | | |  __/ | | | | | (_| |                 \n";
  cout << "  \\____|_|_| |_|\\___|_| |_| |_|\\__,_|              \n";
  cout << "                                                        \n";
  cout << "  " << cinema.getNume() << " - Sistem de Rezervari\n";
  cout << "========================================================\n";
}
void ConsoleUI::afisareFilm(const Film &film) const {
  cout << "  Titlu:   " << film.getTitlu() << "\n";
  cout << "  Tip:     " << film.tipToString() << "\n";
  cout << "  Gen:     " << film.getGen() << "\n";
  cout << "  Durata:  " << film.getDurataMinute() << " min\n";
  cout << std::fixed << std::setprecision(2);
  cout << "  Pret:    " << film.getPretBaza() << " RON\n";
}
void ConsoleUI::afisareHartaLocuri(const Sala &sala) const {
  const auto &matrice = sala.getMatriceLocuri();
  cout << "\n  --- " << sala.getNume() << " (" << sala.getNumarRanduri() << "x"
       << sala.getLocuriPeRand() << ") ---\n";
  cout << "         [ ECRAN ]\n\n";
  cout << "      ";
  for (int j = 1; j <= sala.getLocuriPeRand(); ++j) {
    cout << std::setw(3) << j << " ";
  }
  cout << "\n";
  for (int i = 0; i < sala.getNumarRanduri(); ++i) {
    cout << "  R" << std::setw(2) << (i + 1) << " ";
    for (int j = 0; j < sala.getLocuriPeRand(); ++j) {
      if (matrice[i][j]) {
        cout << " [_]";
      } else {
        cout << " [X]";
      }
    }
    cout << "\n";
  }
  cout << "\n  Legenda: [_] = liber, [X] = ocupat\n";
}
void ConsoleUI::afisareProiectie(const Proiectie &proiectie, int index) const {
  cout << "  " << index << ". [ID:" << proiectie.getId() << "] "
       << proiectie.getFilm()->getTitlu() << " ("
       << proiectie.getFilm()->tipToString() << ") | "
       << proiectie.getSala()->getNume() << " | "
       << proiectie.getDataOraString();
  if (proiectie.aInceput()) {
    cout << " [INCEPUT]";
  }
  cout << "\n";
}
void ConsoleUI::afisareRezervare(const Rezervare &rez) const {
  cout << "  [" << rez.getId() << "] "
       << rez.getProiectie()->getFilm()->getTitlu() << " | "
       << rez.getProiectie()->getSala()->getNume() << " | "
       << rez.getProiectie()->getDataOraString() << " | "
       << "R" << rez.getRand() << "/L" << rez.getLoc() << " | "
       << rez.getTipBilet() << " | " << std::fixed << std::setprecision(2)
       << rez.getPretFinal() << " RON";
  if (rez.esteAnulata()) {
    cout << " [ANULAT]";
  }
  cout << "\n";
}
void ConsoleUI::ruleaza() {
  auto &cinema = Cinematograf::getInstance();
  BiletFactory::incarcaPreturi();
  auto filme = StorageService::incarcaFilme();
  for (auto &f : filme)
    cinema.adaugaFilm(f);
  auto sali = StorageService::incarcaSali();
  for (auto &s : sali)
    cinema.adaugaSala(s);
  auto proiectii =
      StorageService::incarcaProiectii(cinema.getFilme(), cinema.getSali());
  for (auto &p : proiectii)
    cinema.addProiectieDirect(p);
  auto useri = StorageService::incarcaUseri(sali);
  for (auto &u : useri) {
    try {
      cinema.adaugaUser(u);
    } catch (...) {
    }
  }
  auto rezervari = StorageService::incarcaRezervari(cinema.getProiectii());
  cinema.setRezervari(rezervari);
  auto vouchere = StorageService::incarcaVouchere();
  for (auto &v : vouchere)
    cinema.adaugaVoucher(v);
  auto snacks = StorageService::incarcaSnacks();
  for (auto &s : snacks)
    cinema.adaugaProdus(s);
  int maxIdP = 0, maxIdR = 0;
  for (const auto &p : cinema.getProiectii()) {
    if (p->getId() > maxIdP)
      maxIdP = p->getId();
  }
  for (const auto &r : cinema.getRezervari()) {
    try {
      int nr = std::stoi(r.getId().substr(3));
      if (nr > maxIdR)
        maxIdR = nr;
    } catch (...) {
    }
  }
  cinema.setNextIdProiectie(maxIdP + 1);
  cinema.setNextIdRezervare(maxIdR + 1);
  if (!cinema.existaUser("admin")) {
    User admin("admin", User::hashParola("admin123"), RolUser::Admin);
    cinema.adaugaUser(std::make_shared<User>(admin));
  }
  bool running = true;
  while (running) {
    clearScreen();
    afisareHeader();
    cout << "\n  1. Autentificare\n";
    cout << "  2. Inregistrare cont client\n";
    cout << "  0. Iesire\n\n";
    int opt = citesteOptiune(0, 2);
    switch (opt) {
    case 1:
      meniuAutentificare();
      break;
    case 2:
      meniuInregistrare();
      break;
    case 0:
      StorageService::salveazaFilme(cinema.getFilme());
      StorageService::salveazaSali(cinema.getSali());
      StorageService::salveazaProiectii(cinema.getProiectii());
      StorageService::salveazaRezervari(cinema.getRezervari());
      StorageService::salveazaUseri(cinema.getUseri());
      StorageService::salveazaVouchere(cinema.getVouchere());
      StorageService::salveazaSnacks(cinema.getProduse());
      cout << "\n  Date salvate. La revedere!\n";
      running = false;
      break;
    }
  }
}
void ConsoleUI::meniuAutentificare() {
  clearScreen();
  afisareHeader();
  cout << "\n  --- AUTENTIFICARE ---\n\n";
  cout << "  1. Autentificare Client\n";
  cout << "  2. Autentificare Admin\n";
  cout << "  0. Inapoi\n\n";
  int opt = citesteOptiune(0, 2);
  if (opt == 0)
    return;
  string username = citesteString("\n  Username: ");
  string parola = citesteString("  Parola: ");
  try {
    auto &cinema = Cinematograf::getInstance();
    User *user = nullptr;
    if (opt == 1) {
      user = cinema.autentificaClient(username, parola);
      cout << "\n  Bine ati venit, " << user->getUsername() << "!\n";
      meniuClient(*user);
    } else if (opt == 2) {
      user = cinema.autentificaAdmin(username, parola);
      cout << "\n  Bine ati venit, " << user->getUsername() << "!\n";
      meniuAdmin(*user);
    }
  } catch (const AutentificareEsuataExceptie &e) {
    cout << "\n  [EROARE] " << e.what() << "\n";
    pauseScreen();
  }
}
void ConsoleUI::meniuInregistrare() {
  clearScreen();
  afisareHeader();
  cout << "\n  --- INREGISTRARE CONT CLIENT ---\n\n";
  string username = citesteString("  Username: ");
  string parola = citesteString("  Parola: ");
  try {
    auto &cinema = Cinematograf::getInstance();
    User newUser(username, User::hashParola(parola), RolUser::Client);
    cinema.adaugaUser(std::make_shared<User>(newUser));
    cout << "\n  Cont creat cu succes! Va puteti autentifica.\n";
  } catch (const UsernameExistaExceptie &e) {
    cout << "\n  [EROARE] " << e.what() << "\n";
  }
  pauseScreen();
}
void ConsoleUI::meniuAdmin(User &user) {
  bool running = true;
  while (running) {
    clearScreen();
    afisareHeader();
    cout << "  1. Adauga Film\n";
    cout << "  2. Adauga Sala\n";
    cout << "  3. Adauga Proiectie\n";
    cout << "  4. Vezi toate rezervarile\n";
    cout << "  5. Afiseaza harta locuri (sala)\n";
    cout << "  6. Anuleaza o rezervare\n";
    cout << "  7. Vezi filme existente\n";
    cout << "  8. Vezi sali existente\n";
    cout << "  9. Vezi proiectii existente\n";
    cout << " 10. Gestionare vouchere\n";
    cout << " 11. Gestionare snacks\n";
    cout << " 12. POS Bar (Vanzare Rapida)\n";
    cout << "  0. Logout\n\n";
    int opt = citesteOptiune(0, 12);
    switch (opt) {
    case 1:
      adminAdaugaFilm();
      break;
    case 2:
      adminAdaugaSala();
      break;
    case 3:
      adminAdaugaProiectie();
      break;
    case 4:
      adminVeziRezervari();
      break;
    case 5:
      adminAfisareHartaLocuri();
      break;
    case 6:
      adminAnuleazaRezervare();
      break;
    case 7: {
      clearScreen();
      auto &cinema = Cinematograf::getInstance();
      cout << "\n  === LISTA FILME (din filme.csv) ===\n\n";
      if (cinema.getFilme().empty()) {
        cout << "  Nu exista filme.\n";
      } else {
        for (size_t i = 0; i < cinema.getFilme().size(); ++i) {
          cout << "  --- Film #" << (i + 1) << " ---\n";
          afisareFilm(*cinema.getFilme()[i]);
          cout << "\n";
        }
      }
      pauseScreen();
      break;
    }
    case 8: {
      clearScreen();
      auto &cinema = Cinematograf::getInstance();
      cout << "\n  === LISTA SALI (din sali.csv) ===\n\n";
      if (cinema.getSali().empty()) {
        cout << "  Nu exista sali.\n";
      } else {
        for (size_t i = 0; i < cinema.getSali().size(); ++i) {
          const auto &s = cinema.getSali()[i];
          cout << "  " << (i + 1) << ". " << s->getNume() << " ["
               << s->tipToString() << "]"
               << " | " << s->getCapacitateTotala() << " locuri"
               << " | pret x" << s->getMultiplicatorPret() << "\n";
        }
      }
      pauseScreen();
      break;
    }
    case 9: {
      clearScreen();
      auto &cinema = Cinematograf::getInstance();
      const auto &proiectii = cinema.getProiectii();
      cout << "\n  === LISTA PROIECTII ===\n\n";
      if (proiectii.empty()) {
        cout << "  Nu exista proiectii programate.\n";
      } else {
        for (size_t i = 0; i < proiectii.size(); ++i) {
          afisareProiectie(*proiectii[i], static_cast<int>(i + 1));
        }
      }
      pauseScreen();
      break;
    }
    case 10:
      adminGestionareVouchere();
      break;
    case 11:
      adminGestionareSnacks();
      break;
    case 12:
      adminPOSBar();
      break;
    case 0:
      running = false;
      break;
    }
  }
}
void ConsoleUI::meniuClient(User &user) {
  bool running = true;
  while (running) {
    clearScreen();
    afisareHeader();
    cout << "\n  --- MENIU CLIENT (" << user.getUsername() << ") ---\n\n";
    cout << "  1. Vezi programul filmelor\n";
    cout << "  2. Rezerva un loc\n";
    cout << "  3. Istoricul rezervarilor mele\n";
    cout << "  4. Anuleaza o rezervare\n";
    cout << "  5. Pre-comanda Snacks (Sari peste coada)\n";
    cout << "  0. Logout\n\n";
    int opt = citesteOptiune(0, 5);
    switch (opt) {
    case 1:
      clientVeziProgram();
      break;
    case 2:
      clientRezervaLoc(user);
      break;
    case 3:
      clientIstoricRezervari(user);
      break;
    case 4:
      clientAnuleazaRezervare(user);
      break;
    case 5:
      clientPrecomandaSnacks(user);
      break;
    case 0:
      running = false;
      break;
    }
  }
}
void ConsoleUI::adminPOSBar() {
  auto &cinema = Cinematograf::getInstance();
  struct ItemBon {
    std::string nume;
    double pret;
  };
  std::vector<ItemBon> bon;
  double total = 0.0;
  while (true) {
    clearScreen();
    auto &produse = cinema.getProduse();
    cout << "\n  +----------------------------------------------+\n";
    cout << "  |          POS BAR  --  VANZARE RAPIDA         |\n";
    cout << "  +----------------------------------------------+\n\n";
    cout << "  .--- PRODUSE DISPONIBILE ----------------------\n";
    bool existaDisponibil = false;
    for (size_t i = 0; i < produse.size(); ++i) {
      const auto &p = produse[i];
      std::string stocInfo = p.getStoc() > 0
                                 ? "[stoc: " + std::to_string(p.getStoc()) + "]"
                                 : "[EPUIZAT]";
      cout << "  |  " << (i + 1) << ". " << p.getNume();
      cout << std::string(22 - std::min(p.getNume().size(), size_t(22)), ' ');
      cout << std::fixed << std::setprecision(2) << p.getPret() << " RON  "
           << stocInfo << "\n";
      if (p.getStoc() > 0)
        existaDisponibil = true;
    }
    cout << "  '----------------------------------------------\n\n";
    cout << "  .--- BON CURENT --------------------------------\n";
    if (bon.empty()) {
      cout << "  |  (bon gol)\n";
    } else {
      for (const auto &item : bon) {
        cout << "  |  + " << item.nume;
        cout << std::string(21 - std::min(item.nume.size(), size_t(21)), ' ');
        cout << std::fixed << std::setprecision(2) << item.pret << " RON\n";
      }
      cout << "  |  --------------------------------------------\n";
      cout << "  |  TOTAL: " << std::fixed << std::setprecision(2) << total
           << " RON\n";
    }
    cout << "  '----------------------------------------------\n\n";
    if (!existaDisponibil && bon.empty()) {
      cout << "  [!] Nu exista produse disponibile in stoc.\n";
      pauseScreen();
      return;
    }
    cout << "  Index produs (0 = inchide bonul): ";
    int idx;
    cin >> idx;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (idx == 0)
      break;
    if (idx < 1 || idx > static_cast<int>(produse.size())) {
      cout << "\n  [!] Index invalid.\n";
      pauseScreen();
      continue;
    }
    auto &produs = cinema.getProdus(idx - 1);
    if (produs.getStoc() <= 0) {
      cout << "\n  [!] Produsul \"" << produs.getNume() << "\" este epuizat!\n";
      pauseScreen();
      continue;
    }
    produs.setStoc(produs.getStoc() - 1);
    total += produs.getPret();
    bon.push_back({produs.getNume(), produs.getPret()});
  }
  clearScreen();
  cout << "\n  +----------------------------------------------+\n";
  cout << "  |                  BON FISCAL                  |\n";
  cout << "  +----------------------------------------------+\n\n";
  if (bon.empty()) {
    cout << "  Bon gol -- nicio vanzare inregistrata.\n";
  } else {
    for (const auto &item : bon) {
      cout << "  " << item.nume;
      cout << std::string(26 - std::min(item.nume.size(), size_t(26)), ' ');
      cout << std::fixed << std::setprecision(2) << item.pret << " RON\n";
    }
    cout << "\n  ==============================================\n";
    cout << "  DE INCASAT:";
    cout << std::string(15, ' ');
    cout << std::fixed << std::setprecision(2) << total << " RON\n";
    cout << "  ==============================================\n";
    cout << "\n  Va multumim! Pofta buna!\n";
  }
  pauseScreen();
}
void ConsoleUI::clientPrecomandaSnacks(const User &user) {
  clearScreen();
  auto &cinema = Cinematograf::getInstance();
  cout << "\n  +----------------------------------------------+\n";
  cout << "  |      PRE-COMANDA SNACKS - SARI COADA         |\n";
  cout << "  +----------------------------------------------+\n\n";
  auto toateRezervari = cinema.getRezervariUser(user.getUsername());
  std::vector<Rezervare> active;
  for (const auto &r : toateRezervari)
    if (!r.esteAnulata() && !r.getProiectie()->aInceput())
      active.push_back(r);
  if (active.empty()) {
    cout << "  [!] Nu aveti rezervari active pentru filme viitoare.\n";
    cout << "      Faceti mai intai o rezervare pentru a pre-comanda.\n";
    pauseScreen();
    return;
  }
  cout << "  Rezervarile dvs. active:\n\n";
  for (size_t i = 0; i < active.size(); ++i) {
    const auto &r = active[i];
    cout << "  " << (i + 1) << ". [" << r.getId() << "]  "
         << r.getProiectie()->getFilm()->getTitlu() << "\n"
         << "      " << r.getProiectie()->getSala()->getNume() << "  |  "
         << r.getProiectie()->getDataOraString() << "\n"
         << "      Loc: R" << r.getRand() << "/L" << r.getLoc() << "\n\n";
  }
  int rezOpt = citesteOptiune(1, static_cast<int>(active.size()));
  const Rezervare &rezervareAleasa = active[rezOpt - 1];
  struct ItemCos {
    std::string nume;
    double pret;
  };
  std::vector<ItemCos> cos;
  double total = 0.0;
  while (true) {
    clearScreen();
    auto &produse = cinema.getProduse();
    cout << "\n  +----------------------------------------------+\n";
    cout << "  |            COS DE CUMPARATURI                |\n";
    cout << "  |  Rezervare: " << rezervareAleasa.getId();
    size_t idLen = rezervareAleasa.getId().size();
    cout << std::string(31 - std::min(idLen, size_t(31)), ' ') << "|\n";
    cout << "  +----------------------------------------------+\n\n";
    cout << "  .--- CE VA PLACE? -----------------------------\n";
    bool existaDisponibil = false;
    for (size_t i = 0; i < produse.size(); ++i) {
      const auto &p = produse[i];
      cout << "  |  " << (i + 1) << ". " << p.getNume();
      cout << std::string(22 - std::min(p.getNume().size(), size_t(22)), ' ');
      cout << std::fixed << std::setprecision(2) << p.getPret() << " RON";
      if (p.getStoc() > 0) {
        cout << "  [OK]\n";
        existaDisponibil = true;
      } else {
        cout << "  [epuizat]\n";
      }
    }
    cout << "  '----------------------------------------------\n\n";
    cout << "  .--- COSUL TAU --------------------------------\n";
    if (cos.empty()) {
      cout << "  |  (gol -- adauga produse de mai sus)\n";
    } else {
      for (const auto &item : cos) {
        cout << "  |  >> " << item.nume;
        cout << std::string(20 - std::min(item.nume.size(), size_t(20)), ' ');
        cout << std::fixed << std::setprecision(2) << item.pret << " RON\n";
      }
      cout << "  |  --------------------------------------------\n";
      cout << "  |  Total: " << std::fixed << std::setprecision(2) << total
           << " RON\n";
    }
    cout << "  '----------------------------------------------\n\n";
    if (!existaDisponibil && cos.empty()) {
      cout << "  [!] Ne pare rau, stocul este momentan epuizat.\n";
      pauseScreen();
      return;
    }
    cout << "  Adauga in cos (index) sau 0 pentru finalizare: ";
    int idx;
    cin >> idx;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (idx == 0)
      break;
    if (idx < 1 || idx > static_cast<int>(produse.size())) {
      cout << "\n  [!] Index invalid. Incearca din nou.\n";
      pauseScreen();
      continue;
    }
    auto &produs = cinema.getProdus(idx - 1);
    if (produs.getStoc() <= 0) {
      cout << "\n  [!] Ne pare rau, \"" << produs.getNume()
           << "\" s-a epuizat!\n";
      pauseScreen();
      continue;
    }
    produs.setStoc(produs.getStoc() - 1);
    total += produs.getPret();
    cos.push_back({produs.getNume(), produs.getPret()});
    cout << "\n  [OK] \"" << produs.getNume() << "\" adaugat in cos!\n";
    pauseScreen();
  }
  clearScreen();
  if (cos.empty()) {
    cout << "\n  Nu ati adaugat niciun produs. Comanda anulata.\n";
    pauseScreen();
    return;
  }
  cout << "\n  +----------------------------------------------+\n";
  cout << "  |           COMANDA CONFIRMATA!                |\n";
  cout << "  +----------------------------------------------+\n\n";
  cout << "  Rezervare : " << rezervareAleasa.getId() << "\n";
  cout << "  Film      : "
       << rezervareAleasa.getProiectie()->getFilm()->getTitlu() << "\n";
  cout << "  Data/Ora  : " << rezervareAleasa.getProiectie()->getDataOraString()
       << "\n";
  cout << "  Loc       : R" << rezervareAleasa.getRand() << "/L"
       << rezervareAleasa.getLoc() << "\n\n";
  cout << "  .--- SNACKS COMANDATE --------------------------\n";
  for (const auto &item : cos) {
    cout << "  |  " << item.nume;
    cout << std::string(24 - std::min(item.nume.size(), size_t(24)), ' ');
    cout << std::fixed << std::setprecision(2) << item.pret << " RON\n";
  }
  cout << "  |  --------------------------------------------\n";
  cout << "  |  TOTAL DE PLATA LA BAR: " << std::fixed << std::setprecision(2)
       << total << " RON\n";
  cout << "  '----------------------------------------------\n\n";
  cout << "  Ridicati comanda de la bar cu ID-ul rezervarii:\n";
  cout << "  >>> " << rezervareAleasa.getId() << " <<<\n\n";
  cout << "  Pofta buna si film placut!\n";
  pauseScreen();
}
void ConsoleUI::adminAdaugaFilm() {
  clearScreen();
  cout << "\n  --- ADAUGARE FILM ---\n\n";
  string titlu = citesteString("  Titlu: ");
  cout << "  Durata (minute): ";
  int durata;
  cin >> durata;
  cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  cout << "  Tip (1=2D, 2=3D): ";
  int tipInt;
  cin >> tipInt;
  cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  TipFilm tip = (tipInt == 2) ? TipFilm::Film3D : TipFilm::Film2D;
  string gen = citesteString("  Gen (ex: Actiune, SF, Comedie): ");
  cout << "  Pret baza (RON): ";
  double pret;
  cin >> pret;
  cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  auto film = std::make_shared<Film>(titlu, durata, tip, gen, pret);
  Cinematograf::getInstance().adaugaFilm(film);
  cout << "\n  Film adaugat cu succes!\n";
  pauseScreen();
}
void ConsoleUI::adminAdaugaSala() {
  clearScreen();
  cout << "\n  --- ADAUGARE SALA ---\n\n";
  cout << "  Tipuri disponibile:\n";
  cout
      << "    1. Standard  - 10 randuri x 15 locuri (150 total) | pret x1.00\n";
  cout << "       Bilete: Adult, Student, Elev, Pensionar, Personal\n\n";
  cout
      << "    2. IMAX      - 8 randuri x 12 locuri  (96 total)  | pret x1.50\n";
  cout << "       Bilete: Adult, Student, Elev, Pensionar, Personal\n\n";
  cout
      << "    3. VIP       - 4 randuri x 6 fotolii  (24 total)  | pret x2.00\n";
  cout << "       Fotolii duble, spatii intre randuri\n";
  cout << "       Bilete: VIP, Personal\n\n";
  int tipInt = citesteOptiune(1, 3);
  TipSala tipSala;
  switch (tipInt) {
  case 2:
    tipSala = TipSala::IMAX;
    break;
  case 3:
    tipSala = TipSala::VIP;
    break;
  default:
    tipSala = TipSala::Standard;
    break;
  }
  string nume = citesteString("  Nume sala: ");
  auto sala = std::make_shared<Sala>(nume, tipSala);
  Cinematograf::getInstance().adaugaSala(sala);
  cout << "\n  Sala adaugata cu succes!\n";
  cout << "  Tip: " << sala->tipToString()
       << " | Capacitate: " << sala->getCapacitateTotala()
       << " locuri | Multiplicator pret: x" << sala->getMultiplicatorPret()
       << "\n";
  pauseScreen();
}
void ConsoleUI::adminAdaugaProiectie() {
  clearScreen();
  auto &cinema = Cinematograf::getInstance();
  cout << "\n  --- ADAUGARE PROIECTIE ---\n\n";
  const auto &filme = cinema.getFilme();
  if (filme.empty()) {
    cout << "  Nu exista filme! Adaugati mai intai un film.\n";
    pauseScreen();
    return;
  }
  cout << "  Filme disponibile:\n";
  for (size_t i = 0; i < filme.size(); ++i) {
    cout << "    " << (i + 1) << ". " << filme[i]->getTitlu() << " ("
         << filme[i]->tipToString() << ", " << filme[i]->getDurataMinute()
         << " min)\n";
  }
  cout << "\n  Alegeti filmul: ";
  int filmIdx = citesteOptiune(1, static_cast<int>(filme.size())) - 1;
  const auto &sali = cinema.getSali();
  if (sali.empty()) {
    cout << "  Nu exista sali! Adaugati mai intai o sala.\n";
    pauseScreen();
    return;
  }
  cout << "\n  Sali disponibile:\n";
  for (size_t i = 0; i < sali.size(); ++i) {
    cout << "    " << (i + 1) << ". " << sali[i]->getNume() << " ("
         << sali[i]->getCapacitateTotala() << " locuri)\n";
  }
  cout << "\n  Alegeti sala: ";
  int salaIdx = citesteOptiune(1, static_cast<int>(sali.size())) - 1;
  cout << "\n  Format audio:\n";
  cout << "  1. Subtitrat\n";
  cout << "  2. Dublat\n";
  int formatInt = citesteOptiune(1, 2);
  FormatAudio formatAudio =
      (formatInt == 2) ? FormatAudio::Dublat : FormatAudio::Subtitrat;
  string dataOraStr = citesteString("  Data si ora (YYYY-MM-DD HH:MM): ");
  try {
    auto dt = Proiectie::parseDataOra(dataOraStr);
    cinema.adaugaProiectie(filme[filmIdx], sali[salaIdx], dt, formatAudio);
    cout << "\n  Proiectie adaugata cu succes!\n";
  } catch (const DataInTrecutExceptie &e) {
    cout << "\n  [EROARE] " << e.what() << "\n";
  } catch (const std::exception &e) {
    cout << "\n  [EROARE] " << e.what() << "\n";
  }
  pauseScreen();
}
void ConsoleUI::adminVeziRezervari() {
  clearScreen();
  auto &cinema = Cinematograf::getInstance();
  const auto &rezervari = cinema.getRezervari();
  cout << "\n  === TOATE REZERVARILE ===\n\n";
  if (rezervari.empty()) {
    cout << "  Nu exista rezervari.\n";
  } else {
    for (const auto &rez : rezervari) {
      afisareRezervare(rez);
      cout << "     Client: " << rez.getUsernameClient() << "\n";
    }
  }
  pauseScreen();
}
void ConsoleUI::adminAfisareHartaLocuri() {
  clearScreen();
  auto &cinema = Cinematograf::getInstance();
  const auto &proiectii = cinema.getProiectii();
  cout << "\n  === HARTA LOCURI ===\n\n";
  if (proiectii.empty()) {
    cout << "  Nu exista proiectii.\n";
    pauseScreen();
    return;
  }
  cout << "  Proiectii:\n";
  for (size_t i = 0; i < proiectii.size(); ++i) {
    afisareProiectie(*proiectii[i], static_cast<int>(i + 1));
  }
  cout << "\n  Alegeti proiectia: ";
  int idx = citesteOptiune(1, static_cast<int>(proiectii.size())) - 1;
  afisareHartaLocuri(*proiectii[idx]->getSala());
  pauseScreen();
}
void ConsoleUI::adminAnuleazaRezervare() {
  clearScreen();
  auto &cinema = Cinematograf::getInstance();
  cout << "\n  --- ANULARE REZERVARE (ADMIN) ---\n\n";
  const auto &rezervari = cinema.getRezervari();
  if (rezervari.empty()) {
    cout << "  Nu exista rezervari.\n";
    pauseScreen();
    return;
  }
  for (const auto &rez : rezervari) {
    afisareRezervare(rez);
  }
  string idRez = citesteString("\n  Introduceti ID-ul rezervarii de anulat: ");
  try {
    cinema.anuleazaRezervare(idRez, "admin", RolUser::Admin);
    cout << "\n  Rezervare anulata cu succes!\n";
  } catch (const std::exception &e) {
    cout << "\n  [EROARE] " << e.what() << "\n";
  }
  pauseScreen();
}
void ConsoleUI::clientVeziProgram() {
  clearScreen();
  auto &cinema = Cinematograf::getInstance();
  auto viitoare = cinema.getProiectiiViitoare();
  cout << "\n  === PROGRAM CINEMA ===\n\n";
  if (viitoare.empty()) {
    cout << "  Nu exista proiectii viitoare programate.\n";
  } else {
    for (size_t i = 0; i < viitoare.size(); ++i) {
      afisareProiectie(*viitoare[i], static_cast<int>(i + 1));
      cout << "     Gen: " << viitoare[i]->getFilm()->getGen()
           << " | Durata: " << viitoare[i]->getFilm()->getDurataMinute()
           << " min"
           << " | Pret de la: " << std::fixed << std::setprecision(2)
           << viitoare[i]->getFilm()->getPretBaza() << " RON\n\n";
    }
  }
  pauseScreen();
}
void ConsoleUI::clientRezervaLoc(User &user) {
  clearScreen();
  auto &cinema = Cinematograf::getInstance();
  auto viitoare = cinema.getProiectiiViitoare();
  cout << "\n  --- REZERVARE LOC ---\n\n";
  if (viitoare.empty()) {
    cout << "  Nu exista proiectii disponibile.\n";
    pauseScreen();
    return;
  }
  cout << "  Proiectii disponibile:\n\n";
  for (size_t i = 0; i < viitoare.size(); ++i) {
    afisareProiectie(*viitoare[i], static_cast<int>(i + 1));
  }
  cout << "\n  Alegeti proiectia: ";
  int proiIdx = citesteOptiune(1, static_cast<int>(viitoare.size())) - 1;
  auto proiectie = viitoare[proiIdx];
  afisareHartaLocuri(*proiectie->getSala());
  cout << "\n  Randul (1-" << proiectie->getSala()->getNumarRanduri() << "): ";
  int rand;
  cin >> rand;
  cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  cout << "  Locul (1-" << proiectie->getSala()->getLocuriPeRand() << "): ";
  int loc;
  cin >> loc;
  cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  TipSala tipSala = proiectie->getSala()->getTipSala();
  auto tipuriPermise = BiletFactory::getTipuriPermise(tipSala);
  double multSala = proiectie->getSala()->getMultiplicatorPret();
  cout << "\n  Tip bilet disponibil pentru "
       << proiectie->getSala()->tipToString() << ":\n";
  cout << std::fixed << std::setprecision(2);
  for (size_t i = 0; i < tipuriPermise.size(); ++i) {
    double pretExemplu = proiectie->getFilm()->getPretBaza() *
                         tipuriPermise[i].second * multSala;
    cout << "    " << (i + 1) << ". " << tipuriPermise[i].first << " (x"
         << tipuriPermise[i].second << " bilet, x" << multSala << " sala) -> "
         << pretExemplu << " RON\n";
  }
  int tipInt = citesteOptiune(1, static_cast<int>(tipuriPermise.size()));
  std::string tipBilet = tipuriPermise[tipInt - 1].first;
  double pretBaza = BiletFactory::calculeazaPret(
      tipBilet, proiectie->getFilm()->getPretBaza(), multSala);
  cout << std::fixed << std::setprecision(2);
  cout << "\n  Pret initial: " << pretBaza << " RON\n";
  double pretFinal = pretBaza;
  string codVoucher = citesteString("  Cod voucher (ENTER pentru a sari): ");
  if (!codVoucher.empty()) {
    auto &cinema2 = Cinematograf::getInstance();
    Voucher *v = cinema2.gasesteVoucher(codVoucher);
    if (v == nullptr) {
      cout << "  [!] Codul voucher nu exista.\n";
    } else if (!v->esteActiv()) {
      cout << "  [!] Voucherul nu mai este activ.\n";
    } else if (!v->esteValid(tipBilet)) {
      bool ziuaOk = true;
      if (v->getZiuaValida() != ZiuaSaptamanii::Toate) {
        std::time_t acum = std::time(nullptr);
        std::tm *tm = std::localtime(&acum);
        ziuaOk = (tm->tm_wday == Voucher::ziuaToInt(v->getZiuaValida()));
      }
      if (!ziuaOk) {
        cout << "  [!] Voucherul este valabil doar " << v->descriereZiua()
             << ".\n";
      } else {
        cout << "  [!] Voucherul nu este aplicabil pentru tipul de bilet '"
             << tipBilet << "'.\n";
      }
    } else {
      pretFinal = v->aplicaReducere(pretBaza);
      cout << "  [OK] Voucher aplicat: -" << v->getReducereProcent() << "% ("
           << v->descriereZiua() << ")\n";
      cout << "  Pret dupa reducere: " << pretFinal << " RON\n";
    }
  }
  cout << "\n  Pret final: " << pretFinal << " RON\n";
  cout << "  Confirmati rezervarea? (1=Da, 0=Nu): ";
  int confirm;
  cin >> confirm;
  cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  if (confirm != 1) {
    cout << "\n  Rezervare anulata.\n";
    pauseScreen();
    return;
  }
  try {
    auto &rez = cinema.creeazaRezervare(proiectie, rand, loc, tipBilet,
                                        user.getUsername(), pretFinal);
    cout << "\n  ============================================\n";
    cout << "  REZERVARE CONFIRMATA!\n";
    cout << "  ID: " << rez.getId() << "\n";
    if (!codVoucher.empty() && pretFinal < pretBaza) {
      cout << "  Reducere voucher aplicata: " << pretBaza - pretFinal
           << " RON\n";
    }
    cout << "  Biletul a fost salvat in: data/" << rez.getId()
         << "_bilet.txt\n";
    cout << "  ============================================\n";
  } catch (const std::exception &e) {
    cout << "\n  [EROARE] " << e.what() << "\n";
  }
  pauseScreen();
}
void ConsoleUI::clientIstoricRezervari(const User &user) {
  clearScreen();
  auto &cinema = Cinematograf::getInstance();
  auto rezervari = cinema.getRezervariUser(user.getUsername());
  cout << "\n  === ISTORICUL REZERVARILOR ===\n\n";
  if (rezervari.empty()) {
    cout << "  Nu aveti nicio rezervare.\n";
  } else {
    for (const auto &rez : rezervari) {
      afisareRezervare(rez);
    }
  }
  pauseScreen();
}
void ConsoleUI::clientAnuleazaRezervare(const User &user) {
  clearScreen();
  auto &cinema = Cinematograf::getInstance();
  auto rezervari = cinema.getRezervariUser(user.getUsername());
  cout << "\n  --- ANULARE REZERVARE ---\n\n";
  std::vector<Rezervare> active;
  for (const auto &r : rezervari) {
    if (!r.esteAnulata() && !r.getProiectie()->aInceput()) {
      active.push_back(r);
    }
  }
  if (active.empty()) {
    cout << "  Nu aveti rezervari care pot fi anulate.\n";
    cout << "  (Doar rezervarile pentru proiectii viitoare pot fi anulate)\n";
    pauseScreen();
    return;
  }
  cout << "  Rezervari anulabile:\n\n";
  for (const auto &rez : active) {
    afisareRezervare(rez);
  }
  string idRez = citesteString("\n  Introduceti ID-ul rezervarii de anulat: ");
  try {
    cinema.anuleazaRezervare(idRez, user.getUsername(), RolUser::Client);
    cout << "\n  Rezervare anulata cu succes! Locul a fost eliberat.\n";
  } catch (const std::exception &e) {
    cout << "\n  [EROARE] " << e.what() << "\n";
  }
  pauseScreen();
}
void ConsoleUI::afisareVoucher(const Voucher &v) const {
  cout << "  [" << v.getCod() << "] -" << v.getReducereProcent() << "% | "
       << v.descriereZiua() << " | ";
  if (v.esteActiv()) {
    cout << "ACTIV";
  } else {
    cout << "INACTIV";
  }
  const auto &excl = v.getTipuriExcluse();
  if (!excl.empty()) {
    cout << " | Exclus: ";
    for (size_t i = 0; i < excl.size(); ++i) {
      if (i > 0)
        cout << ", ";
      cout << excl[i];
    }
  }
  cout << "\n";
}
void ConsoleUI::adminGestionareVouchere() {
  bool running = true;
  while (running) {
    clearScreen();
    auto &cinema = Cinematograf::getInstance();
    cout << "\n  --- GESTIONARE VOUCHERE ---\n\n";
    cout << "  1. Lista vouchere existente\n";
    cout << "  2. Adauga voucher nou\n";
    cout << "  3. Dezactiveaza un voucher\n";
    cout << "  0. Inapoi\n\n";
    int opt = citesteOptiune(0, 3);
    switch (opt) {
    case 1: {
      clearScreen();
      cout << "\n  === LISTA VOUCHERE ===\n\n";
      const auto &vouchere = cinema.getVouchere();
      if (vouchere.empty()) {
        cout << "  Nu exista vouchere.\n";
      } else {
        for (const auto &v : vouchere) {
          afisareVoucher(v);
        }
      }
      pauseScreen();
      break;
    }
    case 2: {
      clearScreen();
      cout << "\n  --- ADAUGARE VOUCHER ---\n\n";
      string cod = citesteString("  Cod voucher: ");
      if (cinema.existaVoucher(cod)) {
        cout << "\n  [EROARE] Un voucher cu acest cod exista deja!\n";
        pauseScreen();
        break;
      }
      cout << "  Reducere (%): ";
      double reducere;
      cin >> reducere;
      cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      cout << "\n  Ziua valabilitatii:\n";
      cout << "    0. Toate zilele\n";
      cout << "    1. Luni\n";
      cout << "    2. Marti\n";
      cout << "    3. Miercuri\n";
      cout << "    4. Joi\n";
      cout << "    5. Vineri\n";
      cout << "    6. Sambata\n";
      cout << "    7. Duminica\n\n";
      int ziuaOpt = citesteOptiune(0, 7);
      ZiuaSaptamanii ziua;
      if (ziuaOpt == 0) {
        ziua = ZiuaSaptamanii::Toate;
      } else if (ziuaOpt == 7) {
        ziua = ZiuaSaptamanii::Duminica;
      } else {
        ziua = Voucher::intToZiua(ziuaOpt);
      }
      string excluseStr = citesteString(
          "  Tipuri bilet excluse (separate cu |, sau ENTER pt niciunul): ");
      std::vector<std::string> excluse;
      if (!excluseStr.empty()) {
        std::istringstream ssE(excluseStr);
        string tip;
        while (std::getline(ssE, tip, '|')) {
          if (!tip.empty())
            excluse.push_back(tip);
        }
      }
      Voucher v(cod, reducere, ziua, excluse, true);
      cinema.adaugaVoucher(v);
      cout << "\n  Voucher adaugat cu succes!\n";
      afisareVoucher(v);
      pauseScreen();
      break;
    }
    case 3: {
      clearScreen();
      cout << "\n  --- DEZACTIVARE VOUCHER ---\n\n";
      const auto &vouchere = cinema.getVouchere();
      if (vouchere.empty()) {
        cout << "  Nu exista vouchere.\n";
        pauseScreen();
        break;
      }
      for (const auto &v : vouchere) {
        afisareVoucher(v);
      }
      string cod = citesteString("\n  Cod voucher de dezactivat: ");
      Voucher *v = cinema.gasesteVoucher(cod);
      if (v == nullptr) {
        cout << "  [EROARE] Voucherul nu exista!\n";
      } else if (!v->esteActiv()) {
        cout << "  [!] Voucherul este deja inactiv.\n";
      } else {
        v->dezactiveaza();
        cout << "  Voucher dezactivat cu succes!\n";
      }
      pauseScreen();
      break;
    }
    case 0:
      running = false;
      break;
    }
  }
}
void ConsoleUI::adminGestionareSnacks() {
  bool running = true;
  while (running) {
    clearScreen();
    auto &cinema = Cinematograf::getInstance();
    cout << "\n  --- GESTIONARE SNACKS ---\n\n";
    cout << "  1. Lista produse\n";
    cout << "  2. Adauga produs\n";
    cout << "  3. Actualizeaza stoc\n";
    cout << "  0. Inapoi\n\n";
    int opt = citesteOptiune(0, 3);
    switch (opt) {
    case 1: {
      clearScreen();
      cout << "\n  === LISTA SNACKS ===\n\n";
      const auto &produse = cinema.getProduse();
      if (produse.empty()) {
        cout << "  Nu exista produse.\n";
      } else {
        for (size_t i = 0; i < produse.size(); ++i) {
          const auto &p = produse[i];
          cout << "  " << (i + 1) << ". " << p.getNume() << " | "
               << p.getDescriere() << " | " << std::fixed
               << std::setprecision(2) << p.getPret() << " RON"
               << " | Stoc: " << p.getStoc()
               << (p.esteDisponibil() ? "" : " [EPUIZAT]") << "\n";
        }
      }
      pauseScreen();
      break;
    }
    case 2: {
      clearScreen();
      cout << "\n  --- ADAUGARE PRODUS ---\n\n";
      string nume = citesteString("  Nume: ");
      string desc = citesteString("  Descriere: ");
      cout << "  Pret (RON): ";
      double pret;
      cin >> pret;
      cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      cout << "  Stoc initial: ";
      int stoc;
      cin >> stoc;
      cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      cinema.adaugaProdus(ProdusConsumabil(nume, desc, pret, stoc));
      cout << "\n  Produs adaugat cu succes!\n";
      pauseScreen();
      break;
    }
    case 3: {
      clearScreen();
      cout << "\n  --- ACTUALIZARE STOC ---\n\n";
      const auto &produse = cinema.getProduse();
      if (produse.empty()) {
        cout << "  Nu exista produse.\n";
        pauseScreen();
        break;
      }
      for (size_t i = 0; i < produse.size(); ++i)
        cout << "  " << (i + 1) << ". " << produse[i].getNume()
             << " (stoc: " << produse[i].getStoc() << ")\n";
      int idx = citesteOptiune(1, static_cast<int>(produse.size()));
      cout << "  Stoc nou: ";
      int stocNou;
      cin >> stocNou;
      cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      cinema.getProdus(idx - 1).setStoc(stocNou);  
      cout << "\n  Stoc actualizat!\n";
      pauseScreen();
      break;
    }
    case 0:
      running = false;
      break;
    }
  }
}
