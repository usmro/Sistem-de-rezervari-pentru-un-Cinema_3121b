#include "Cinematograf.h"
#include "CrowServer.h"

int main() {
  // Datele din CSV sunt incarcate in CrowServer::ruleaza() (vezi CrowServer.cpp).
  // Fiecare modificare (rezervare, anulare, actiuni admin) se salveaza IMEDIAT
  // in CSV in handler-ul respectiv. De aceea NU mai salvam tot la iesire:
  // un server cu date invechite in memorie ar suprascrie datele proaspete de pe
  // disc la inchidere (cauza pierderilor de date de dinainte).
  Cinematograf::getInstance();

  CrowServer server;
  server.ruleaza();
  return 0;
}
