#pragma once
#include <string>
#include <memory>
class Bilet {
public:
    virtual ~Bilet() = default;
    virtual double calculeazaPret(double pretBaza) const = 0;
    virtual std::string getNumeTip() const = 0;
    virtual double getMultiplicator() const = 0;
};
class BiletConfigurabil : public Bilet {
private:
    std::string numeTip;
    double multiplicator;
public:
    BiletConfigurabil(const std::string& numeTip, double multiplicator);
    double calculeazaPret(double pretBaza) const override;
    std::string getNumeTip() const override;
    double getMultiplicator() const override;
};
