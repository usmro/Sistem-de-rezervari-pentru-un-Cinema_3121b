#pragma once
#include <string>
class ProdusConsumabil {
public:
  ProdusConsumabil(const std::string &nume, const std::string &descriere,
                   double pret, int stoc);
  const std::string &getNume() const;
  const std::string &getDescriere() const;
  double getPret() const;
  int getStoc() const;
  void setStoc(int stoc);
  bool esteDisponibil() const;
  std::string toCSV() const;
  static ProdusConsumabil fromCSV(const std::string &linie);

private:
  std::string nume;
  std::string descriere;
  double pret;
  int stoc;
};
