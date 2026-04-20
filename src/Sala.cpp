#include "Sala.h"

using std::string;
using std::endl;
using std::cout;

Sala::Sala(string numeSala, int numarRanduri, int locuriPeRand) : numeSala(numeSala), numarRanduri(numarRanduri), locuriPeRand(locuriPeRand){
    matriceLocuri = vector<vector<bool>>(numarRanduri, vector<bool>(locuriPeRand, true));
}

string Sala::GetNume() const{
    return numeSala;
}

int Sala::getCapacitateTotala() const {
    return numarRanduri * locuriPeRand;
}

void Sala::afisareLocuri() const{
    cout << "\n--- " << numeSala << " ---\n";
    cout << "\t\t [ ECRAN ] \n\n";

    for(int i = 0; i < numarRanduri; ++i) {
        cout << "R" << i + 1 << ":\t";
        for(int j = 0; j < locuriPeRand; ++j) {
            if(matriceLocuri[i][j]){
                cout << "[_] ";
            }
            else{
                cout << "[X] ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

void Sala::rezervaLoc(int rand, int loc){
    if(rand < 1 || rand > numarRanduri || loc < 1 || loc > locuriPeRand){
        throw IndexInvalidExceptie();
    }

    if(!matriceLocuri[rand - 1][loc - 1]){
        throw LocOcupatExceptie();
    }

    matriceLocuri[rand - 1][loc - 1] = false;
}