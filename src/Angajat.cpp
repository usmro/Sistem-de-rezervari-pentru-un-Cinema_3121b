#include "Angajat.h"
Angajat::Angajat(const std::string &username, const std::string &parola,
                 const std::string &programLucru,
                 std::shared_ptr<Sala> salaResponsabil)
    : User(username, parola, RolUser::Angajat), programLucru(programLucru),
      salaResponsabil(std::move(salaResponsabil)) {}
const std::string &Angajat::getProgramLucru() const { return programLucru; }
std::shared_ptr<Sala> Angajat::getSalaResponsabil() const {
  return salaResponsabil;
}
void Angajat::setSalaResponsabil(std::shared_ptr<Sala> sala) {
  salaResponsabil = std::move(sala);
}
std::string Angajat::toCSV() const {
  std::string sala = salaResponsabil ? salaResponsabil->getNume() : "-";
  return getUsername() + "," + getParola() + ",Angajat," + programLucru + "," +
         sala;
}
