#pragma once
#include <stdexcept>
#include <string>
class LocOcupatExceptie : public std::runtime_error {
public:
    LocOcupatExceptie() : std::runtime_error("Locul selectat este deja ocupat!") {}
};
class IndexInvalidExceptie : public std::out_of_range {
public:
    IndexInvalidExceptie() : std::out_of_range("Randul sau locul introdus nu exista in sala!") {}
};
class LocLiber : public std::runtime_error {
public:
    LocLiber() : std::runtime_error("Locul selectat este deja liber!") {}
};
class AutentificareEsuataExceptie : public std::runtime_error {
public:
    AutentificareEsuataExceptie() : std::runtime_error("Nume de utilizator sau parola incorecta!") {}
};
class UsernameExistaExceptie : public std::runtime_error {
public:
    UsernameExistaExceptie() : std::runtime_error("Acest nume de utilizator exista deja!") {}
};
class ProiectieInexistentaExceptie : public std::runtime_error {
public:
    ProiectieInexistentaExceptie() : std::runtime_error("Proiectia selectata nu exista!") {}
};
class DataInTrecutExceptie : public std::invalid_argument {
public:
    DataInTrecutExceptie() : std::invalid_argument("Nu se poate adauga o proiectie cu data in trecut!") {}
};
class FisierInvalidExceptie : public std::runtime_error {
public:
    explicit FisierInvalidExceptie(const std::string& numeFisier)
        : std::runtime_error("Nu s-a putut deschide fisierul: " + numeFisier) {}
};
class RezervareInexistentaExceptie : public std::runtime_error {
public:
    RezervareInexistentaExceptie() : std::runtime_error("Rezervarea selectata nu exista!") {}
};
class AnulareImposibilaExceptie : public std::runtime_error {
public:
    AnulareImposibilaExceptie() : std::runtime_error("Proiectia a inceput deja, nu se mai poate anula!") {}
};
