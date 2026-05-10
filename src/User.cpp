#include "User.h"
#include <functional>
#include <sstream>
User::User(const std::string &username, const std::string &parola, RolUser rol,
           bool areCardFidelitate)
    : username(username), parola(parola), rol(rol),
      areCardFidelitate(areCardFidelitate) {}
bool User::getAreCardFidelitate() const { return areCardFidelitate; }
void User::setAreCardFidelitate(bool val) { areCardFidelitate = val; }
const std::string &User::getUsername() const { return username; }
const std::string &User::getParola() const { return parola; }
RolUser User::getRol() const { return rol; }
std::string User::rolToString() const {
  switch (rol) {
  case RolUser::Admin:
    return "Admin";
  case RolUser::Angajat:
    return "Angajat";
  default:
    return "Client";
  }
}
bool User::verificaParola(const std::string &parolaInput) const {
  return parola == hashParola(parolaInput);
}
std::string User::hashParola(const std::string &parola) {
  unsigned long hash = 5381;
  for (char c : parola) {
    hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
  }
  return std::to_string(hash);
}
std::string User::toCSV() const {
  return username + "," + parola + "," + rolToString() + "," +
         (areCardFidelitate ? "1" : "0");
}
User User::fromCSV(const std::string &linie) {
  std::istringstream ss(linie);
  std::string user, pass, rolStr, cardStr;
  std::getline(ss, user, ',');
  std::getline(ss, pass, ',');
  std::getline(ss, rolStr, ',');
  std::getline(ss, cardStr, ',');
  RolUser rol;
  if (rolStr == "Admin")
    rol = RolUser::Admin;
  else if (rolStr == "Angajat")
    rol = RolUser::Angajat;
  else
    rol = RolUser::Client;
  bool card = (cardStr == "1");
  return User(user, pass, rol, card);
}
