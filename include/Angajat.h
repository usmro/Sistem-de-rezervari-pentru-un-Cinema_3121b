#pragma once
#include "Sala.h"
#include "User.h"
#include <memory>
class Angajat : public User {
public:
  Angajat(const std::string &username, const std::string &parola,
          const std::string &programLucru,
          std::shared_ptr<Sala> salaResponsabil = nullptr);
  const std::string &getProgramLucru() const;
  std::shared_ptr<Sala> getSalaResponsabil() const;
  void setSalaResponsabil(std::shared_ptr<Sala> sala);
  std::string toCSV() const override;
private:
  std::string programLucru;
  std::shared_ptr<Sala> salaResponsabil;
};
