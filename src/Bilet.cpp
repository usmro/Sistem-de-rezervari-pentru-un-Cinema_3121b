#include "Bilet.h"
#include "BiletFactory.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
BiletConfigurabil::BiletConfigurabil(const std::string& numeTip, double multiplicator)
    : numeTip(numeTip), multiplicator(multiplicator) {}
double BiletConfigurabil::calculeazaPret(double pretBaza) const {
    return pretBaza * multiplicator;
}
std::string BiletConfigurabil::getNumeTip() const { return numeTip; }
double BiletConfigurabil::getMultiplicator() const { return multiplicator; }
std::vector<std::pair<std::string, double>> BiletFactory::tipuriBilete;
bool BiletFactory::incarcat = false;
void BiletFactory::incarcaPreturi(const std::string& fisier) {
    tipuriBilete.clear();
    std::ifstream fin(fisier);
    if (!fin.is_open()) {
        tipuriBilete = {
            {"Adult",     1.00},
            {"Student",   0.70},
            {"Elev",      0.60},
            {"Pensionar", 0.50},
            {"VIP",       1.00},
            {"Personal",  0.30}
        };
        salveazaPreturi(fisier);
        incarcat = true;
        return;
    }
    std::string linie;
    while (std::getline(fin, linie)) {
        if (linie.empty()) continue;
        std::istringstream ss(linie);
        std::string tip, multStr;
        std::getline(ss, tip, ',');
        std::getline(ss, multStr);
        try {
            tipuriBilete.emplace_back(tip, std::stod(multStr));
        } catch (...) {
            std::cerr << "[WARN] Linie invalida in preturi.csv: " << linie << "\n";
        }
    }
    incarcat = true;
}
void BiletFactory::salveazaPreturi(const std::string& fisier) {
    std::ofstream fout(fisier);
    if (!fout.is_open()) return;
    for (const auto& [tip, mult] : tipuriBilete) {
        fout << tip << "," << mult << "\n";
    }
}
std::unique_ptr<Bilet> BiletFactory::creeazaBilet(const std::string& tip) {
    if (!incarcat) incarcaPreturi();
    for (const auto& [nume, mult] : tipuriBilete) {
        if (nume == tip)
            return std::make_unique<BiletConfigurabil>(nume, mult);
    }
    throw std::invalid_argument("Tip bilet necunoscut: " + tip);
}
double BiletFactory::calculeazaPret(const std::string& tip, double pretBaza, double multSala) {
    auto bilet = creeazaBilet(tip);
    return pretBaza * bilet->getMultiplicator() * multSala;
}
const std::vector<std::pair<std::string, double>>& BiletFactory::getTipuri() {
    if (!incarcat) incarcaPreturi();
    return tipuriBilete;
}
std::vector<std::pair<std::string, double>> BiletFactory::getTipuriPermise(TipSala tipSala) {
    if (!incarcat) incarcaPreturi();
    std::vector<std::pair<std::string, double>> rezultat;
    for (const auto& [tip, mult] : tipuriBilete) {
        if (tipSala == TipSala::VIP) {
            if (tip == "VIP" || tip == "Personal") {
                rezultat.emplace_back(tip, mult);
            }
        } else {
            if (tip != "VIP") {
                rezultat.emplace_back(tip, mult);
            }
        }
    }
    return rezultat;
}
double BiletFactory::getMultiplicator(const std::string& tip) {
    if (!incarcat) incarcaPreturi();
    for (const auto& [nume, mult] : tipuriBilete) {
        if (nume == tip) return mult;
    }
    return 1.0;
}
bool BiletFactory::existaTip(const std::string& tip) {
    if (!incarcat) incarcaPreturi();
    for (const auto& [nume, mult] : tipuriBilete) {
        if (nume == tip) return true;
    }
    return false;
}
