#include "CrowServer.h"
#include "ApiSerializer.h"
#include "Cinematograf.h"
#include "Film.h"
#include "FormatAudio.h"
#include "Proiectie.h"
#include "StorageService.h"
#include <crow.h>
#include <fstream>
#include <iomanip>
#include <vector>

struct CORSMiddleware {
  struct context {};

  void before_handle(crow::request &req, crow::response &res, context &) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");

    if (req.method == crow::HTTPMethod::OPTIONS) {
      res.code = 204;
      res.end();
    }
  }

  void after_handle(crow::request &, crow::response &res, context &) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");
  }
};

void CrowServer::ruleaza() {
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

  if (!cinema.existaUser("admin")) {
    cinema.adaugaUser(std::make_shared<User>(
        "admin", User::hashParola("admin123"), RolUser::Admin));
  }

  int maxIdP = 0, maxIdR = 0;
  for (const auto &p : cinema.getProiectii())
    if (p->getId() > maxIdP)
      maxIdP = p->getId();
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

  crow::App<CORSMiddleware> app;

  CROW_ROUTE(app, "/api/login")
      .methods(crow::HTTPMethod::POST)([&cinema](const crow::request &req) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        try {
          auto body = crow::json::load(req.body);
          if (!body) {
            res.code = 400;
            res.write(ApiSerializer::eroare("JSON invalid"));
            return res;
          }
          std::string username = body["username"].s();
          std::string parola = body["parola"].s();
          std::string tip = body["tip"].s();

          User *user = nullptr;
          if (tip == "admin") {
            user = cinema.autentificaAdmin(username, parola);
          } else {
            user = cinema.autentificaClient(username, parola);
          }

          res.code = 200;
          res.write("{\"status\":\"ok\",\"user\":" +
                    ApiSerializer::userToJson(*user) + "}");
        } catch (const std::exception &e) {
          res.code = 401;
          res.write(ApiSerializer::eroare(e.what()));
        }
        return res;
      });

  CROW_ROUTE(app, "/api/register")
      .methods(crow::HTTPMethod::POST)([&cinema](const crow::request &req) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        try {
          auto body = crow::json::load(req.body);
          if (!body) {
            res.code = 400;
            res.write(ApiSerializer::eroare("JSON invalid"));
            return res;
          }
          std::string username = body["username"].s();
          std::string parola = body["parola"].s();

          if (username.empty() || parola.empty()) {
            res.code = 400;
            res.write(
                ApiSerializer::eroare("Username si parola sunt obligatorii"));
            return res;
          }

          auto user = std::make_shared<User>(username, User::hashParola(parola),
                                             RolUser::Client);
          cinema.adaugaUser(user);
          StorageService::salveazaUseri(cinema.getUseri());

          res.code = 201;
          res.write(ApiSerializer::ok("Cont creat cu succes!"));
        } catch (const std::exception &e) {
          res.code = 409;
          res.write(ApiSerializer::eroare(e.what()));
        }
        return res;
      });

  CROW_ROUTE(app, "/api/filme")
      .methods(crow::HTTPMethod::GET)([&cinema](const crow::request &) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        res.write(ApiSerializer::filmeToJson(cinema.getFilme()));
        return res;
      });

  CROW_ROUTE(app, "/api/proiectii/viitoare")
      .methods(crow::HTTPMethod::GET)([&cinema](const crow::request &) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        res.write(
            ApiSerializer::proiectiiToJson(cinema.getProiectiiViitoare()));
        return res;
      });

  CROW_ROUTE(app, "/api/proiectii/vip")
      .methods(crow::HTTPMethod::GET)([&cinema](const crow::request &) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        res.write(ApiSerializer::proiectiiToJson(cinema.getProiectiiVIP()));
        return res;
      });

  CROW_ROUTE(app, "/api/snacks")
      .methods(crow::HTTPMethod::GET)([&cinema](const crow::request &) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        res.write(ApiSerializer::snacksToJson(cinema.getProduse()));
        return res;
      });

  CROW_ROUTE(app, "/api/verifica-voucher")
      .methods(crow::HTTPMethod::POST)([&cinema](const crow::request &req) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        try {
          auto body = crow::json::load(req.body);
          std::string cod = body["cod"].s();
          std::string tip = body["tipBilet"].s();

          Voucher *v = cinema.gasesteVoucher(cod);
          if (v && v->esteActiv() && v->esteValid(tip)) {
            res.write("{\"valid\":true, \"reducere\":" +
                      std::to_string(v->getReducereProcent()) + "}");
          } else {
            res.write("{\"valid\":false}");
          }
        } catch (...) {
          res.write("{\"valid\":false}");
        }
        return res;
      });

  CROW_ROUTE(app, "/api/rezerva")
      .methods(crow::HTTPMethod::POST)([&cinema](const crow::request &req) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        try {
          auto body = crow::json::load(req.body);
          if (!body) {
            res.code = 400;
            res.write(ApiSerializer::eroare("JSON invalid"));
            return res;
          }

          int idProiectie = body["idProiectie"].i();
          int rand = body["rand"].i();
          int loc = body["loc"].i();
          std::string tipBilet = body["tipBilet"].s();
          std::string username = body["username"].s();

          std::shared_ptr<Proiectie> proiectieDorita = nullptr;
          for (const auto &p : cinema.getProiectiiViitoare()) {
            if (p->getId() == idProiectie) {
              proiectieDorita = p;
              break;
            }
          }

          if (!proiectieDorita) {
            res.code = 404;
            res.write(ApiSerializer::eroare(
                "Proiectia nu a fost gasita sau a inceput deja."));
            return res;
          }

          // ===== LOGICA REPARATA PENTRU VOUCHERE =====
          double pretVoucher = 0.0;
          if (body.has("codVoucher") && body["codVoucher"].s() != "") {
            std::string codVoucher = body["codVoucher"].s();
            Voucher *v = cinema.gasesteVoucher(codVoucher);
            if (v && v->esteActiv() && v->esteValid(tipBilet)) {
              // Backend-ul tau stie sa calculeze pretul de baza perfect
              double multSala =
                  proiectieDorita->getSala()->getMultiplicatorPret();
              double pretBazaCalc = BiletFactory::calculeazaPret(
                  tipBilet, proiectieDorita->getFilm()->getPretBaza(),
                  multSala);
              pretVoucher = v->aplicaReducere(pretBazaCalc);
            } else {
              res.code = 400;
              res.write(ApiSerializer::eroare(
                  "Voucher invalid, expirat sau neaplicabil pentru acest tip "
                  "de bilet."));
              return res;
            }
          }
          // ===========================================

          Rezervare &nouaRezervare = cinema.creeazaRezervare(
              proiectieDorita, rand, loc, tipBilet, username, pretVoucher);
          StorageService::salveazaRezervari(cinema.getRezervari());

          res.code = 201;
          res.write("{\"status\":\"ok\",\"mesaj\":\"Rezervare confirmata!\", "
                    "\"idRezervare\":\"" +
                    nouaRezervare.getId() + "\"}");

        } catch (const std::exception &e) {
          res.code = 400;
          res.write(ApiSerializer::eroare(e.what()));
        }
        return res;
      });

  CROW_ROUTE(app, "/api/admin/useri")
      .methods(crow::HTTPMethod::GET)([&cinema](const crow::request &) {
        crow::response res;
        res.add_header("Content-Type", "application/json");

        // Construim manual JSON-ul pentru utilizatori
        std::string json = "[";
        const auto &useri = cinema.getUseri();
        for (size_t i = 0; i < useri.size(); ++i) {
          json += "{";
          json += "\"username\":\"" + useri[i]->getUsername() + "\",";
          json += "\"rol\":\"" + useri[i]->rolToString() + "\",";
          json +=
              "\"areCardFidelitate\":" +
              std::string(useri[i]->getAreCardFidelitate() ? "true" : "false");
          json += "}";
          if (i < useri.size() - 1)
            json += ",";
        }
        json += "]";

        res.write(json);
        return res;
      });

  // GET /api/admin/rezervari
  CROW_ROUTE(app, "/api/admin/rezervari")
      .methods(crow::HTTPMethod::GET)([&cinema](const crow::request &) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        std::string json = "[";
        const auto &rezervari = cinema.getRezervari();
        for (size_t i = 0; i < rezervari.size(); ++i) {
          const auto &r = rezervari[i];
          auto proj = r.getProiectie();
          json += "{";
          json += "\"id\":\"" + r.getId() + "\",";
          json += "\"usernameClient\":\"" + r.getUsernameClient() + "\",";
          json += "\"titluFilm\":\"" +
                  (proj ? proj->getFilm()->getTitlu() : "") + "\",";
          json +=
              "\"dataOra\":\"" + (proj ? proj->getDataOraString() : "") + "\",";
          json += "\"rand\":" + std::to_string(r.getRand()) + ",";
          json += "\"loc\":" + std::to_string(r.getLoc()) + ",";
          json += "\"tipBilet\":\"" + r.getTipBilet() + "\",";
          json += "\"pretFinal\":" + std::to_string(r.getPretFinal()) + ",";
          json +=
              "\"anulata\":" + std::string(r.esteAnulata() ? "true" : "false");
          json += "}";
          if (i < rezervari.size() - 1)
            json += ",";
        }
        json += "]";
        res.write(json);
        return res;
      });

  // POST /api/admin/anuleaza
  CROW_ROUTE(app, "/api/admin/anuleaza")
      .methods(crow::HTTPMethod::POST)([&cinema](const crow::request &req) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        try {
          auto body = crow::json::load(req.body);
          if (!body) {
            res.code = 400;
            res.write(ApiSerializer::eroare("JSON invalid"));
            return res;
          }
          std::string idRezervare = body["idRezervare"].s();
          cinema.anuleazaRezervare(idRezervare, "admin", RolUser::Admin);
          StorageService::salveazaRezervari(cinema.getRezervari());
          res.code = 200;
          res.write(ApiSerializer::ok("Rezervare anulata!"));
        } catch (const std::exception &e) {
          res.code = 400;
          res.write(ApiSerializer::eroare(e.what()));
        }
        return res;
      });

  // POST /api/admin/card-fidelitate
  CROW_ROUTE(app, "/api/admin/card-fidelitate")
      .methods(crow::HTTPMethod::POST)([&cinema](const crow::request &req) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        try {
          auto body = crow::json::load(req.body);
          if (!body) {
            res.code = 400;
            res.write(ApiSerializer::eroare("JSON invalid"));
            return res;
          }
          std::string username = body["username"].s();
          bool areCard = body["areCardFidelitate"].b();
          User *user = cinema.gasesteUser(username);
          if (!user) {
            res.code = 404;
            res.write(ApiSerializer::eroare("User negasit"));
            return res;
          }
          user->setAreCardFidelitate(areCard);
          StorageService::salveazaUseri(cinema.getUseri());
          res.code = 200;
          res.write(ApiSerializer::ok("Card fidelitate actualizat!"));
        } catch (const std::exception &e) {
          res.code = 400;
          res.write(ApiSerializer::eroare(e.what()));
        }
        return res;
      });

  // POST /api/admin/filme
  CROW_ROUTE(app, "/api/admin/filme")
      .methods(crow::HTTPMethod::POST)([&cinema](const crow::request &req) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        try {
          auto body = crow::json::load(req.body);
          if (!body) {
            res.code = 400;
            res.write(ApiSerializer::eroare("JSON invalid"));
            return res;
          }
          std::string titlu = body["titlu"].s();
          std::string gen = body["gen"].s();
          int durata = body["durata"].i();
          double pretBaza = body["pretBaza"].d();
          std::string tipStr = body["tip"].s();
          TipFilm tip = (tipStr == "3D") ? TipFilm::Film3D : TipFilm::Film2D;
          if (cinema.gasesteFilm(titlu)) {
            res.code = 409;
            res.write(
                ApiSerializer::eroare("Film cu acest titlu exista deja!"));
            return res;
          }
          auto film = std::make_shared<Film>(titlu, durata, tip, gen, pretBaza);
          cinema.adaugaFilm(film);
          StorageService::salveazaFilme(cinema.getFilme());
          res.code = 201;
          res.write(ApiSerializer::ok("Film adaugat cu succes!"));
        } catch (const std::exception &e) {
          res.code = 400;
          res.write(ApiSerializer::eroare(e.what()));
        }
        return res;
      });

  // POST /api/admin/proiectii
  CROW_ROUTE(app, "/api/admin/proiectii")
      .methods(crow::HTTPMethod::POST)([&cinema](const crow::request &req) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        try {
          auto body = crow::json::load(req.body);
          if (!body) {
            res.code = 400;
            res.write(ApiSerializer::eroare("JSON invalid"));
            return res;
          }
          std::string titluFilm = body["titluFilm"].s();
          std::string numeSala = body["numeSala"].s();
          std::string dataOraStr = body["dataOra"].s();
          std::string formatStr = body["formatAudio"].s();
          auto film = cinema.gasesteFilm(titluFilm);
          if (!film) {
            res.code = 404;
            res.write(ApiSerializer::eroare("Film negasit"));
            return res;
          }
          auto sala = cinema.gasesteSala(numeSala);
          if (!sala) {
            res.code = 404;
            res.write(ApiSerializer::eroare("Sala negasita"));
            return res;
          }
          auto dt = Proiectie::parseDataOra(dataOraStr);
          FormatAudio format = (formatStr == "Dublat") ? FormatAudio::Dublat
                                                       : FormatAudio::Subtitrat;
          cinema.adaugaProiectie(film, sala, dt, format);
          StorageService::salveazaProiectii(cinema.getProiectii());
          res.code = 201;
          res.write(ApiSerializer::ok("Proiectie adaugata!"));
        } catch (const std::exception &e) {
          res.code = 400;
          res.write(ApiSerializer::eroare(e.what()));
        }
        return res;
      });

  // POST /api/admin/snacks/refill — adauga stoc la un produs
  CROW_ROUTE(app, "/api/admin/snacks/refill")
      .methods(crow::HTTPMethod::POST)([&cinema](const crow::request &req) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        try {
          auto body = crow::json::load(req.body);
          if (!body) { res.code = 400; res.write(ApiSerializer::eroare("JSON invalid")); return res; }
          int idx = body["id"].i() - 1;
          int cantitate = body["cantitate"].i();
          if (cantitate < 1) { res.code = 400; res.write(ApiSerializer::eroare("Cantitate invalida")); return res; }
          if (idx < 0 || idx >= (int)cinema.getProduse().size()) {
            res.code = 404; res.write(ApiSerializer::eroare("Produs negasit")); return res;
          }
          auto &produs = cinema.getProdus(idx);
          produs.setStoc(produs.getStoc() + cantitate);
          StorageService::salveazaSnacks(cinema.getProduse());
          res.code = 200;
          res.write("{\"status\":\"ok\",\"stocNou\":" + std::to_string(produs.getStoc()) + "}");
        } catch (...) {
          res.code = 500; res.write(ApiSerializer::eroare("Eroare server"));
        }
        return res;
      });

  // ================================================================
  // POST /api/comanda-snacks
  // Primeste o lista de produse si le scade din stocul C++
  // ================================================================
  CROW_ROUTE(app, "/api/comanda-snacks")
      .methods(crow::HTTPMethod::POST)([&cinema](const crow::request &req) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        try {
          auto body = crow::json::load(req.body);
          if (!body) {
            res.code = 400;
            res.write("{\"status\":\"eroare\",\"mesaj\":\"JSON invalid\"}");
            return res;
          }

          std::string idRezervare = body["idRezervare"].s();
          auto snacks = body["snacks"];

          // Decrement stock and collect ordered items for the ticket
          struct SnackLine { std::string nume; double pret; int qty; };
          std::vector<SnackLine> linii;
          double totalSnacks = 0.0;

          for (const auto &item : snacks) {
            int idx = item["id"].i() - 1;
            int qty = item["qty"].i();
            if (idx >= 0 && idx < (int)cinema.getProduse().size()) {
              auto &produs = cinema.getProdus(idx);
              if (produs.getStoc() >= qty) {
                produs.setStoc(produs.getStoc() - qty);
              }
              double subtotal = produs.getPret() * qty;
              totalSnacks += subtotal;
              linii.push_back({produs.getNume(), produs.getPret(), qty});
            }
          }

          StorageService::salveazaSnacks(cinema.getProduse());

          // Append snacks section to the reservation .txt file
          if (!idRezervare.empty() && !linii.empty()) {
            std::string numeFisier = "data/" + idRezervare + "_bilet.txt";
            std::ofstream fout(numeFisier, std::ios::app);
            if (fout.is_open()) {
              fout << "\n--------------------------------------------\n";
              fout << "  SNACKS PRE-COMANDA  (Skip the Line)        \n";
              fout << "--------------------------------------------\n";
              fout << std::fixed << std::setprecision(2);
              for (const auto &l : linii) {
                fout << "  " << l.qty << "x  " << l.nume
                     << "  @  " << l.pret << " RON"
                     << "  =  " << (l.pret * l.qty) << " RON\n";
              }
              fout << "--------------------------------------------\n";
              fout << "  Total snacks:  " << totalSnacks << " RON\n";
              fout << "--------------------------------------------\n";
              fout << "  Prezinta ID-ul " << idRezervare
                   << " la ghiseul\n";
              fout << "  \"Pre-Comenzi\" pentru a ridica comanda.\n";
              fout << "============================================\n";
            }
          }

          res.code = 200;
          res.write("{\"status\":\"ok\"}");
        } catch (...) {
          res.code = 500;
          res.write("{\"status\":\"eroare\"}");
        }
        return res;
      });

  CROW_ROUTE(app, "/api/proiectii/<int>/locuri")
      .methods(crow::HTTPMethod::GET)([&cinema](int idProiectie) {
        crow::response res;
        res.add_header("Content-Type", "application/json");

        std::string json = "[";
        bool primul = true;
        for (const auto &r : cinema.getRezervari()) {
          if (!r.esteAnulata() && r.getProiectie()->getId() == idProiectie) {
            if (!primul)
              json += ",";
            json += "{\"rand\":" + std::to_string(r.getRand()) +
                    ",\"loc\":" + std::to_string(r.getLoc()) + "}";
            primul = false;
          }
        }
        json += "]";

        res.write(json);
        return res;
      });

  app.port(8080).multithreaded().run();
}