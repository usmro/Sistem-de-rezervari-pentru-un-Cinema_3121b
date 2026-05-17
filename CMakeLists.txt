cmake_minimum_required(VERSION 3.15)
project(CinemaProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# vcpkg toolchain (doar pe Windows)
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

find_package(Crow CONFIG REQUIRED)

include_directories(include)

set(BACKEND_SOURCES
    src/Film.cpp
    src/Sala.cpp
    src/Rezervare.cpp
    src/Cinematograf.cpp
    src/Proiectie.cpp
    src/Bilet.cpp
    src/User.cpp
    src/Angajat.cpp
    src/StorageService.cpp
    src/Voucher.cpp
    src/ProdusConsumabil.cpp
)

add_executable(CinemaApp
    src/main.cpp
    src/ConsoleUI.cpp
    ${BACKEND_SOURCES}
)

add_executable(CinemaServer
    src/main_crow.cpp
    src/CrowServer.cpp
    ${BACKEND_SOURCES}
)

target_link_libraries(CinemaServer PRIVATE Crow::Crow)

# ws2_32 si mswsock doar pe Windows
if(WIN32)
    target_link_libraries(CinemaServer PRIVATE ws2_32 mswsock)
endif()

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/data)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/web)