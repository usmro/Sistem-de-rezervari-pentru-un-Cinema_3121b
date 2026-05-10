#pragma once
#include "Film.h"
#include "FormatAudio.h"
#include "Sala.h"
#include <ctime>
#include <memory>
#include <string>
class Proiectie {
public:
  Proiectie(int idProiectie, std::shared_ptr<Film> film,
            std::shared_ptr<Sala> sala, const std::tm &dataOra,
            FormatAudio formatAudio = FormatAudio::Subtitrat);
  int getId() const;
  std::shared_ptr<Film> getFilm() const;
  std::shared_ptr<Sala> getSala() const;
  const std::tm &getDataOra() const;
  std::string getDataOraString() const;
  FormatAudio getFormatAudio() const;
  std::string getFormatAudioString() const;
  bool aInceput() const;
  std::string toCSV() const;
  static std::tm parseDataOra(const std::string &str);
  static std::string formatDataOra(const std::tm &dt);
private:
  int idProiectie;
  std::shared_ptr<Film> film;
  std::shared_ptr<Sala> sala;
  std::tm dataOra;
  FormatAudio formatAudio;
};
