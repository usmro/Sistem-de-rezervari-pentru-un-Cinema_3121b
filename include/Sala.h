#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

using std::string;
using std::vector;

class LocOcupatExceptie: public std::runtime_error{
    public:
    LocOcupatExceptie() : std::runtime_error("Locul selectat este deja ocupat!"){}

};

class IndexInvalidExceptie: public std::out_of_range{
    public:
    IndexInvalidExceptie(): std::out_of_range("Randul sau locul introdus nu exista in sala! "){}
};

class Sala{
    private:
        string numeSala;
        int numarRanduri; 
        int locuriPeRand;

        vector<vector<bool>> matriceLocuri;

    public:
        Sala(string numeSala, int numarRanduri, int locuriPeRand);
        string GetNume() const; 
        int getCapacitateTotala() const; 
        void afisareLocuri() const; 
        void rezervaLoc(int rand, int loc);
};