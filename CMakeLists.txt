cmake_minimum_required(VERSION 3.10)
project(CinemaProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(CinemaApp src/main.cpp src/Film.cpp src/Sala.cpp src/Rezervare.cpp src/Cinematograf.cpp)