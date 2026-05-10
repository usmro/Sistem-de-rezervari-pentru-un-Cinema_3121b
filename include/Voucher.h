#pragma once
#include <string>
#include <vector>
enum class ZiuaSaptamanii {
    Toate      = -1,
    Duminica   = 0,
    Luni       = 1,
    Marti      = 2,
    Miercuri   = 3,
    Joi        = 4,
    Vineri     = 5,
    Sambata    = 6
};
class Voucher {
private:
    std::string cod;
    double reducereProcent;        
    ZiuaSaptamanii ziuaValida;     
    std::vector<std::string> tipuriExcluse;  
    bool activ;
public:
    Voucher(const std::string& cod, double reducereProcent,
            ZiuaSaptamanii ziuaValida,
            const std::vector<std::string>& tipuriExcluse,
            bool activ = true);
    const std::string& getCod() const;
    double getReducereProcent() const;
    ZiuaSaptamanii getZiuaValida() const;
    const std::vector<std::string>& getTipuriExcluse() const;
    bool esteActiv() const;
    bool esteValid(const std::string& tipBilet) const;
    double aplicaReducere(double pret) const;
    std::string descriereZiua() const;
    void dezactiveaza();
    std::string toCSV() const;
    static Voucher fromCSV(const std::string& linie);
    static int ziuaToInt(ZiuaSaptamanii z);
    static ZiuaSaptamanii intToZiua(int i);
    static std::string ziuaToString(ZiuaSaptamanii z);
};
