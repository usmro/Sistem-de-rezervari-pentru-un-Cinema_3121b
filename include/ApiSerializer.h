#pragma once
#include "Cinematograf.h"
#include "Film.h"
#include "ProdusConsumabil.h"
#include "Proiectie.h"
#include "Rezervare.h"
#include "User.h"
#include "Voucher.h"
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

inline std::string jsonEscape(const std::string &s) {
  std::string out;
  for (char c : s) {
    if (c == '"')
      out += "\\\"";
    else if (c == '\\')
      out += "\\\\";
    else if (c == '\n')
      out += "\\n";
    else if (c == '\r')
      out += "\\r";
    else if (c == '\t')
      out += "\\t";
    else
      out += c;
  }
  return out;
}

struct ApiSerializer {

  static std::string filmToJson(const Film &f) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "{"
       << "\"titlu\":\"" << jsonEscape(f.getTitlu()) << "\","
       << "\"durata\":" << f.getDurataMinute() << ","
       << "\"tip\":\"" << f.tipToString() << "\","
       << "\"gen\":\"" << jsonEscape(f.getGen()) << "\","
       << "\"pretBaza\":" << f.getPretBaza() << "}";
    return ss.str();
  }

  static std::string
  filmeToJson(const std::vector<std::shared_ptr<Film>> &filme) {
    std::string r = "[";
    for (size_t i = 0; i < filme.size(); ++i) {
      if (i > 0)
        r += ",";
      r += filmToJson(*filme[i]);
    }
    return r + "]";
  }

  static std::string salaToJson(const Sala &s) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "{"
       << "\"nume\":\"" << jsonEscape(s.getNume()) << "\","
       << "\"tip\":\"" << s.tipToString() << "\","
       << "\"randuri\":" << s.getNumarRanduri() << ","
       << "\"locuriPeRand\":" << s.getLocuriPeRand() << ","
       << "\"capacitate\":" << s.getCapacitateTotala() << ","
       << "\"multiplicatorPret\":" << s.getMultiplicatorPret() << "}";
    return ss.str();
  }

  static std::string proiectieToJson(const Proiectie &p) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "{"
       << "\"id\":" << p.getId() << ","
       << "\"film\":" << filmToJson(*p.getFilm()) << ","
       << "\"sala\":" << salaToJson(*p.getSala()) << ","
       << "\"dataOra\":\"" << p.getDataOraString() << "\","
       << "\"formatAudio\":\"" << p.getFormatAudioString() << "\","
       << "\"aInceput\":" << (p.aInceput() ? "true" : "false") << "}";
    return ss.str();
  }

  static std::string
  proiectiiToJson(const std::vector<std::shared_ptr<Proiectie>> &proiectii) {
    std::string r = "[";
    for (size_t i = 0; i < proiectii.size(); ++i) {
      if (i > 0)
        r += ",";
      r += proiectieToJson(*proiectii[i]);
    }
    return r + "]";
  }

  static std::string rezervareToJson(const Rezervare &r) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "{"
       << "\"id\":\"" << r.getId() << "\","
       << "\"proiectieId\":" << r.getProiectie()->getId() << ","
       << "\"film\":\"" << jsonEscape(r.getProiectie()->getFilm()->getTitlu())
       << "\","
       << "\"sala\":\"" << jsonEscape(r.getProiectie()->getSala()->getNume())
       << "\","
       << "\"dataOra\":\"" << r.getProiectie()->getDataOraString() << "\","
       << "\"rand\":" << r.getRand() << ","
       << "\"loc\":" << r.getLoc() << ","
       << "\"tipBilet\":\"" << jsonEscape(r.getTipBilet()) << "\","
       << "\"pretFinal\":" << r.getPretFinal() << ","
       << "\"username\":\"" << jsonEscape(r.getUsernameClient()) << "\","
       << "\"anulata\":" << (r.esteAnulata() ? "true" : "false") << "}";
    return ss.str();
  }

  static std::string rezervariToJson(const std::vector<Rezervare> &rezervari) {
    std::string r = "[";
    for (size_t i = 0; i < rezervari.size(); ++i) {
      if (i > 0)
        r += ",";
      r += rezervareToJson(rezervari[i]);
    }
    return r + "]";
  }

  static std::string userToJson(const User &u) {
    std::ostringstream ss;
    ss << "{"
       << "\"username\":\"" << jsonEscape(u.getUsername()) << "\","
       << "\"rol\":\"" << u.rolToString() << "\","
       << "\"cardFidelitate\":" << (u.getAreCardFidelitate() ? "true" : "false")
       << "}";
    return ss.str();
  }

  static std::string snackToJson(const ProdusConsumabil &p) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "{"
       << "\"nume\":\"" << jsonEscape(p.getNume()) << "\","
       << "\"descriere\":\"" << jsonEscape(p.getDescriere()) << "\","
       << "\"pret\":" << p.getPret() << ","
       << "\"stoc\":" << p.getStoc() << ","
       << "\"disponibil\":" << (p.esteDisponibil() ? "true" : "false") << "}";
    return ss.str();
  }

  static std::string
  snacksToJson(const std::vector<ProdusConsumabil> &produse) {
    std::string r = "[";
    for (size_t i = 0; i < produse.size(); ++i) {
      if (i > 0)
        r += ",";
      r += snackToJson(produse[i]);
    }
    return r + "]";
  }

  static std::string hartaLocuriToJson(const Sala &sala) {
    std::ostringstream ss;
    const auto &matrice = sala.getMatriceLocuri();
    ss << "{"
       << "\"sala\":\"" << jsonEscape(sala.getNume()) << "\","
       << "\"randuri\":" << sala.getNumarRanduri() << ","
       << "\"locuriPeRand\":" << sala.getLocuriPeRand() << ","
       << "\"locuri\":[";
    for (int i = 0; i < sala.getNumarRanduri(); ++i) {
      if (i > 0)
        ss << ",";
      ss << "[";
      for (int j = 0; j < sala.getLocuriPeRand(); ++j) {
        if (j > 0)
          ss << ",";
        ss << (matrice[i][j] ? "true" : "false");
      }
      ss << "]";
    }
    ss << "]}";
    return ss.str();
  }

  static std::string ok(const std::string &mesaj) {
    return "{\"status\":\"ok\",\"mesaj\":\"" + jsonEscape(mesaj) + "\"}";
  }

  static std::string eroare(const std::string &mesaj) {
    return "{\"status\":\"eroare\",\"mesaj\":\"" + jsonEscape(mesaj) + "\"}";
  }
};