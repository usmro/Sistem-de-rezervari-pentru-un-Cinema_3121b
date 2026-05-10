#pragma once
#include <string>
#include <vector>
enum class RolUser { Admin, Client, Angajat };
class User {
private:
  std::string username;
  std::string parola;  
  RolUser rol;
  bool areCardFidelitate;
public:
  User(const std::string &username, const std::string &parola, RolUser rol,
       bool areCardFidelitate = false);
  virtual ~User() = default;
  const std::string &getUsername() const;
  const std::string &getParola() const;
  RolUser getRol() const;
  bool getAreCardFidelitate() const;
  void setAreCardFidelitate(bool val);
  std::string rolToString() const;
  bool verificaParola(const std::string &parolaInput) const;
  static std::string hashParola(const std::string &parola);
  virtual std::string toCSV() const;
  static User fromCSV(const std::string &linie);
};
