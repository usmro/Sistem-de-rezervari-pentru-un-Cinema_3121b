#include "Cinematograf.h"
#include "ConsoleUI.h"
#include "StorageService.h"
#include <csignal>

void salveazaLaIesire() {
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
  std::atexit(salveazaLaIesire);
  std::signal(SIGINT, [](int) {
    salveazaLaIesire();
    std::exit(0);
  });
  std::signal(SIGTERM, [](int) {
    salveazaLaIesire();
    std::exit(0);
  });

  ConsoleUI ui;
  ui.ruleaza();
  return 0;
}