#include "Cinematograf.h"

using std::cout;

Cinematograf::Cinematograf(string nume) : nume(nume){}

void Cinematograf::adaugaSala(Sala* sala) {
    sali.push_back(sala);
}

void Cinematograf::afisareSali() const{
    cout << "\n=== CINEMATOGRAF: " << nume << " ===\n";
    cout << "Sali disponibile (" << sali.size() << "):\n";

    for (size_t i = 0; i < sali.size(); ++i) {
        cout << " - " << sali[i]->GetNume() << " (Capacitate: " << sali[i]->getCapacitateTotala() << " locuri)\n";
    }

    cout << "=============================\n";
}

string Cinematograf::getNume() const {
    return nume;
}