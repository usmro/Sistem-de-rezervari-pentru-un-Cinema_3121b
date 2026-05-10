#include "Proiectie.h"
#include "Exceptii.h"
#include <cstring>
#include <iomanip>
#include <sstream>
Proiectie::Proiectie(int idProiectie, std::shared_ptr<Film> film,
                     std::shared_ptr<Sala> sala, const std::tm &dataOra,
                     FormatAudio formatAudio)
    : idProiectie(idProiectie), film(std::move(film)), sala(std::move(sala)),
      dataOra(dataOra), formatAudio(formatAudio) {}
int Proiectie::getId() const { return idProiectie; }
std::shared_ptr<Film> Proiectie::getFilm() const { return film; }
std::shared_ptr<Sala> Proiectie::getSala() const { return sala; }
const std::tm &Proiectie::getDataOra() const { return dataOra; }
FormatAudio Proiectie::getFormatAudio() const { return formatAudio; }
std::string Proiectie::getFormatAudioString() const {
  return formatAudioToString(formatAudio);
}
std::string Proiectie::getDataOraString() const {
  return formatDataOra(dataOra);
}
bool Proiectie::aInceput() const {
  std::time_t acum = std::time(nullptr);
  std::tm copie = dataOra;
  std::time_t timpProiectie = std::mktime(&copie);
  return std::difftime(acum, timpProiectie) > 0;
}
std::string Proiectie::toCSV() const {
  return std::to_string(idProiectie) + "," + film->getTitlu() + "," +
         sala->getNume() + "," + getDataOraString() + "," +
         formatAudioToString(formatAudio);
}
std::tm Proiectie::parseDataOra(const std::string &str) {
  std::tm dt = {};
  std::istringstream ss(str);
  ss >> std::get_time(&dt, "%Y-%m-%d %H:%M");
  if (ss.fail()) {
    throw std::invalid_argument(
        "Format data/ora invalid! Folositi: YYYY-MM-DD HH:MM");
  }
  dt.tm_isdst = -1;
  return dt;
}
std::string Proiectie::formatDataOra(const std::tm &dt) {
  std::ostringstream ss;
  ss << std::put_time(&dt, "%Y-%m-%d %H:%M");
  return ss.str();
}
