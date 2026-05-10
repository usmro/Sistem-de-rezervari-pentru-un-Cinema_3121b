#pragma once
#include <string>
#include <vector>
#include "Exceptii.h"
enum class TipSala {
    Standard,   
    IMAX,       
    VIP         
};
class Sala {
private:
    std::string numeSala;
    TipSala tipSala;
    int numarRanduri;
    int locuriPeRand;
    double multiplicatorPret;
    std::vector<std::vector<bool>> matriceLocuri;
    void aplicaConfiguratieTip();
public:
    Sala(const std::string& numeSala, TipSala tipSala);
    Sala(const std::string& numeSala, TipSala tipSala,
         int numarRanduri, int locuriPeRand);
    const std::string& getNume() const;
    TipSala getTipSala() const;
    int getNumarRanduri() const;
    int getLocuriPeRand() const;
    int getCapacitateTotala() const;
    double getMultiplicatorPret() const;
    bool esteLocDisponibil(int rand, int loc) const;
    const std::vector<std::vector<bool>>& getMatriceLocuri() const;
    std::string tipToString() const;
    static TipSala tipFromString(const std::string& s);
    void rezervaLoc(int rand, int loc);
    void elibereazaLoc(int rand, int loc);
    void reseteazaLocuri();
    std::string toCSV() const;
    static Sala fromCSV(const std::string& linie);
};
