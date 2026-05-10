#include "CrowServer.h"
#include "ApiSerializer.h"
#include "Cinematograf.h"
#include "StorageService.h"
#include "User.h"
#include <crow.h>

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

  // ================================================================
  // POST /api/login
  // ================================================================
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

  // ================================================================
  // POST /api/register
  // ================================================================
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

  // ================================================================
  // GET /api/filme
  // ================================================================
  CROW_ROUTE(app, "/api/filme")
      .methods(crow::HTTPMethod::GET)([&cinema](const crow::request &) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        res.write(ApiSerializer::filmeToJson(cinema.getFilme()));
        return res;
      });

  // ================================================================
  // GET /api/proiectii/viitoare
  // ================================================================
  CROW_ROUTE(app, "/api/proiectii/viitoare")
      .methods(crow::HTTPMethod::GET)([&cinema](const crow::request &) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        res.write(
            ApiSerializer::proiectiiToJson(cinema.getProiectiiViitoare()));
        return res;
      });

  // ================================================================
  // GET /api/proiectii/vip
  // ================================================================
  CROW_ROUTE(app, "/api/proiectii/vip")
      .methods(crow::HTTPMethod::GET)([&cinema](const crow::request &) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        res.write(ApiSerializer::proiectiiToJson(cinema.getProiectiiVIP()));
        return res;
      });

  // ================================================================
  // GET /api/snacks
  // ================================================================
  CROW_ROUTE(app, "/api/snacks")
      .methods(crow::HTTPMethod::GET)([&cinema](const crow::request &) {
        crow::response res;
        res.add_header("Content-Type", "application/json");
        res.write(ApiSerializer::snacksToJson(cinema.getProduse()));
        return res;
      });

  app.port(8080).multithreaded().run();
}