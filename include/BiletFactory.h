#pragma once
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include "Bilet.h"
#include "Sala.h"
class BiletFactory {
private:
    static std::vector<std::pair<std::string, double>> tipuriBilete;
    static bool incarcat;
public:
    static void incarcaPreturi(const std::string& fisier = "data/preturi.csv");
    static void salveazaPreturi(const std::string& fisier = "data/preturi.csv");
    static std::unique_ptr<Bilet> creeazaBilet(const std::string& tip);
    static double calculeazaPret(const std::string& tip, double pretBaza, double multSala = 1.0);
    static const std::vector<std::pair<std::string, double>>& getTipuri();
    static std::vector<std::pair<std::string, double>> getTipuriPermise(TipSala tipSala);
    static double getMultiplicator(const std::string& tip);
    static bool existaTip(const std::string& tip);
};
