#include "Voucher.h"
#include <ctime>
#include <sstream>
#include <stdexcept>

Voucher::Voucher(const std::string &cod, double reducereProcent,
                 ZiuaSaptamanii ziuaValida,
                 const std::vector<std::string> &tipuriExcluse, bool activ)
    : cod(cod), reducereProcent(reducereProcent), ziuaValida(ziuaValida),
      tipuriExcluse(tipuriExcluse), activ(activ) {}
const std::string &Voucher::getCod() const { return cod; }
double Voucher::getReducereProcent() const { return reducereProcent; }
ZiuaSaptamanii Voucher::getZiuaValida() const { return ziuaValida; }
const std::vector<std::string> &Voucher::getTipuriExcluse() const {
  return tipuriExcluse;
}
bool Voucher::esteActiv() const { return activ; }
void Voucher::dezactiveaza() { activ = false; }
bool Voucher::esteValid(const std::string &tipBilet) const {
  if (!activ)
    return false;
  for (const auto &excl : tipuriExcluse) {
    if (excl == tipBilet)
      return false;
  }
  if (ziuaValida != ZiuaSaptamanii::Toate) {
    std::time_t acum = std::time(nullptr);
    std::tm *tmLocal = std::localtime(&acum);
    int ziuaAzi = tmLocal->tm_wday;
    if (ziuaAzi != ziuaToInt(ziuaValida))
      return false;
  }
  return true;
}
double Voucher::aplicaReducere(double pret) const {
  return pret * (1.0 - reducereProcent / 100.0);
}
std::string Voucher::descriereZiua() const { return ziuaToString(ziuaValida); }
std::string Voucher::toCSV() const {
  std::string excl;
  for (size_t i = 0; i < tipuriExcluse.size(); ++i) {
    if (i > 0)
      excl += "|";
    excl += tipuriExcluse[i];
  }
  if (excl.empty())
    excl = "-";
  return cod + "," + std::to_string(reducereProcent) + "," +
         std::to_string(ziuaToInt(ziuaValida)) + "," + excl + "," +
         (activ ? "1" : "0");
}
Voucher Voucher::fromCSV(const std::string &linie) {
  std::istringstream ss(linie);
  std::string codStr, reducStr, ziuaStr, excluseStr, activStr;
  std::getline(ss, codStr, ',');
  std::getline(ss, reducStr, ',');
  std::getline(ss, ziuaStr, ',');
  std::getline(ss, excluseStr, ',');
  std::getline(ss, activStr, ',');
  double reduc = std::stod(reducStr);
  ZiuaSaptamanii ziua = intToZiua(std::stoi(ziuaStr));
  bool act = (activStr == "1");
  std::vector<std::string> excl;
  if (excluseStr != "-" && !excluseStr.empty()) {
    std::istringstream ssE(excluseStr);
    std::string tip;
    while (std::getline(ssE, tip, '|')) {
      if (!tip.empty())
        excl.push_back(tip);
    }
  }
  return Voucher(codStr, reduc, ziua, excl, act);
}
int Voucher::ziuaToInt(ZiuaSaptamanii z) { return static_cast<int>(z); }
ZiuaSaptamanii Voucher::intToZiua(int i) {
  if (i == -1)
    return ZiuaSaptamanii::Toate;
  if (i >= 0 && i <= 6)
    return static_cast<ZiuaSaptamanii>(i);
  return ZiuaSaptamanii::Toate;
}
std::string Voucher::ziuaToString(ZiuaSaptamanii z) {
  switch (z) {
  case ZiuaSaptamanii::Toate:
    return "Toate zilele";
  case ZiuaSaptamanii::Luni:
    return "Luni";
  case ZiuaSaptamanii::Marti:
    return "Marti";
  case ZiuaSaptamanii::Miercuri:
    return "Miercuri";
  case ZiuaSaptamanii::Joi:
    return "Joi";
  case ZiuaSaptamanii::Vineri:
    return "Vineri";
  case ZiuaSaptamanii::Sambata:
    return "Sambata";
  case ZiuaSaptamanii::Duminica:
    return "Duminica";
  }
  return "Toate zilele";
}
