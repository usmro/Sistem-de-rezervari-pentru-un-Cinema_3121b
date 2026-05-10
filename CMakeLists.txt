cmake_minimum_required(VERSION 3.10)
project(CinemaProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(CinemaApp
    src/main.cpp
    src/Film.cpp
    src/Sala.cpp
    src/Rezervare.cpp
    src/Cinematograf.cpp
    src/Proiectie.cpp
    src/Bilet.cpp
    src/User.cpp
    src/StorageService.cpp
    src/ConsoleUI.cpp
    src/Voucher.cpp
    src/ProdusConsumabil.cpp
    src/Angajat.cpp
)

# Cream directorul data/ la build daca nu exista
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/data)