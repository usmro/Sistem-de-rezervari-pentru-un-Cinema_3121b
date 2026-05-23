#include "Cinematograf.h"
#include "CrowServer.h"
#include "StorageService.h"
#include <csignal>
#include <cstdlib>

void salveazaLaIesire() {
  static bool dejaSalvat = false;
  if (dejaSalvat)
    return;
  dejaSalvat = true;

  auto &c = Cinematograf::getInstance();
  StorageService::salveazaFilme(c.getFilme());
  StorageService::salveazaSali(c.getSali());
  StorageService::salveazaProiectii(c.getProiectii());
  StorageService::salveazaRezervari(c.getRezervari());
  StorageService::salveazaUseri(c.getUseri());
  StorageService::salveazaVouchere(c.getVouchere());
  StorageService::salveazaSnacks(c.getProduse());
}

int main() {
  // Datele din CSV sunt încărcate în CrowServer::ruleaza() (vezi CrowServer.cpp)
  Cinematograf::getInstance();

  std::atexit(salveazaLaIesire);

  std::signal(SIGINT, [](int) {
    salveazaLaIesire();
    std::exit(0);
  });
  std::signal(SIGTERM, [](int) {
    salveazaLaIesire();
    std::exit(0);
  });

  CrowServer server;
  server.ruleaza();
  return 0;
}