#pragma once
#include <string>
enum class FormatAudio { Subtitrat, Dublat };
inline std::string formatAudioToString(FormatAudio format) {
  return (format == FormatAudio::Dublat) ? "Dublat" : "Subtitrat";
}
inline FormatAudio formatAudioFromString(const std::string &s) {
  return (s == "Dublat") ? FormatAudio::Dublat : FormatAudio::Subtitrat;
}
