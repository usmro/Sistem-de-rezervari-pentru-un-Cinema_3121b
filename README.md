# Sistem de Rezervări pentru un Cinema

Aplicație C++ orientată pe obiecte (OOP) pentru gestionarea completă a rezervărilor într-un cinematograf.
Proiect realizat de Mandrescu Matei, sgr. 3121b.

## Obiectivele Proiectului
- Gestionarea entităților principale: `Film`, `Sala`, `Proiectie`, `Rezervare`, `User`.
- Implementarea conceptelor OOP: Încapsulare, Moștenire, Polimorfism, Clasă Abstractă.
- **Design Patterns**: Singleton (`Cinematograf`), Factory (`BiletFactory`).
- Separarea logicii de business de interfața cu utilizatorul (`ConsoleUI`).
- Sistem de autentificare cu roluri diferențiate (Admin / Client).
- Persistența datelor în fișiere CSV.
- Sistem de prețuri configurabil extern (fără recompilare).
- Tratarea excepțiilor custom pentru validări.
- Validare temporală: proiecțiile nu pot fi în trecut, rezervările nu se pot anula după începerea filmului.

## Tehnologii Utilizate
- **Limbaj:** C++ (Standard C++17)
- **Sistem de Build:** CMake (minim 3.10) / g++
- **IDE:** Visual Studio Code
- **Version Control:** Git & GitHub

## Structura Proiectului
```
├── include/               # Headerele (.h)
│   ├── Film.h             # Clasa Film (titlu, durată, tip, gen, preț)
│   ├── Sala.h             # Clasa Sala (matrice de locuri, TipSala)
│   ├── Proiectie.h        # Clasa Proiectie (Film + Sala + Data/Ora)
│   ├── Rezervare.h        # Clasa Rezervare (bilet, preț, loc)
│   ├── Bilet.h            # Clasă abstractă Bilet + BiletConfigurabil
│   ├── BiletFactory.h     # Factory Pattern - creează bilete dinamic
│   ├── User.h             # Clasa User (username, parolă, rol, cardFidelitate)
│   ├── Angajat.h          # Clasa Angajat (moștenește User, cu program și sală)
│   ├── Voucher.h          # Clasa Voucher (cod, reducere, validare)
│   ├── FormatAudio.h      # Enum pentru Format Audio (Subtitrat/Dublat)
│   ├── ProdusConsumabil.h # Clasa pentru snacks și băuturi
│   ├── Cinematograf.h     # Singleton - orchestrează totul
│   ├── StorageService.h   # Persistență CSV (citire/scriere fișiere)
│   ├── ConsoleUI.h        # Interfața consolă (meniuri interactive)
│   └── Exceptii.h         # Excepții custom centralizate
├── src/                   # Implementările (.cpp)
│   ├── main.cpp
│   ├── Film.cpp
│   ├── Sala.cpp
│   ├── Proiectie.cpp
│   ├── Rezervare.cpp
│   ├── Bilet.cpp
│   ├── User.cpp
│   ├── Angajat.cpp
│   ├── Voucher.cpp
│   ├── ProdusConsumabil.cpp
│   ├── Cinematograf.cpp
│   ├── StorageService.cpp
│   └── ConsoleUI.cpp
├── data/                  # Fișiere de date (CSV)
│   ├── filme.csv          # Lista filmelor
│   ├── sali.csv           # Lista sălilor
│   ├── proiectii.csv      # Proiecțiile programate
│   ├── rezervari.csv      # Istoricul rezervărilor
│   ├── useri.csv          # Conturile utilizatorilor (admin, clienți, angajați)
│   ├── preturi.csv        # Configurația prețurilor pe tip bilet
│   ├── vouchere.csv       # Voucherele de reducere
│   └── snacks.csv         # Inventar produse consumabile
├── tests/                 # Teste unitare (41 teste)
│   └── test_all.cpp
├── CMakeLists.txt
├── CLAUDE.md              # Context AI pentru asistență
├── DOCUMENTATIE.md        # Documentație tehnică detaliată
└── README.md
```

## Cum se compilează și rulează

### Linux (recomandat de profesor)
```bash
# Compilare cu CMake
mkdir -p build && cd build
cmake ..
make

# Rulare
./CinemaApp
```

Sau compilare directă cu g++:
```bash
g++ -std=c++17 -Iinclude -o CinemaApp \
    src/main.cpp src/Film.cpp src/Sala.cpp src/Rezervare.cpp \
    src/Cinematograf.cpp src/Proiectie.cpp src/Bilet.cpp \
    src/User.cpp src/Angajat.cpp src/StorageService.cpp \
    src/ConsoleUI.cpp src/Voucher.cpp src/ProdusConsumabil.cpp

./CinemaApp
```

### Windows
```powershell
# Cu g++ (MinGW/MSYS2)
g++ -std=c++17 -Iinclude -o build/CinemaApp.exe src/main.cpp src/Film.cpp src/Sala.cpp src/Rezervare.cpp src/Cinematograf.cpp src/Proiectie.cpp src/Bilet.cpp src/User.cpp src/Angajat.cpp src/StorageService.cpp src/ConsoleUI.cpp src/Voucher.cpp src/ProdusConsumabil.cpp

.\build\CinemaApp.exe
```

### Rulare Teste Unitare
```bash
# Compilare teste
g++ -std=c++17 -Iinclude -o tests/test_all.exe tests/test_all.cpp \
    src/Film.cpp src/Sala.cpp src/Proiectie.cpp src/Rezervare.cpp \
    src/Bilet.cpp src/User.cpp src/Angajat.cpp src/Voucher.cpp \
    src/ProdusConsumabil.cpp src/Cinematograf.cpp src/StorageService.cpp

# Rulare (41 teste unitare)
./tests/test_all.exe
```

## Credențiale implicite
- **Admin**: username `admin`, parolă `admin123` (creat automat la prima rulare)
- Clienții se pot înregistra singuri din meniul principal.

## Fișiere de date (CSV)

Toate datele sunt persistente — se salvează automat la ieșirea din aplicație (opțiunea `0 - Iesire`).

| Fișier | Conținut | Exemplu linie |
|---|---|---|
| `filme.csv` | titlu, durată, tip, gen, preț | `Dune: Part Two,155,3D,SF,35.00` |
| `sali.csv` | nume, tip, rânduri, locuri/rând | `Sala IMAX,IMAX,8,12` |
| `proiectii.csv` | id, film, sală, dată+oră | `1,Dune: Part Two,Sala IMAX,2026-05-01 20:00` |
| `rezervari.csv` | id, idProi, rând, loc, tip, preț, user, anulat | `REZ1,1,3,5,Adult,35.00,matei,0` |
| `useri.csv` | username, parolă (hash), rol, cardFidelitate | `admin,407908580,Admin` |
| `vouchere.csv` | cod, reducere%, ziua, tipuri excluse, activ | `MARTI5,5.0,2,Personal,1` |
| `snacks.csv` | id, nume, pret, stoc | `S1,Popcorn Mare,15.50,10` |

### Tipuri de Sală

La adăugarea unei săli, admin-ul alege **tipul** — configurația este automată:

| Tip | Rânduri | Locuri/rând | Total | Multiplicator | Bilete permise |
|-----|---------|-------------|-------|---------------|----------------|
| Standard | 10 | 15 | 150 | ×1.00 | Adult, Student, Elev, Pensionar, Personal |
| IMAX | 8 | 12 | 96 | ×1.50 | Adult, Student, Elev, Pensionar, Personal |
| VIP | 4 | 6 | 24 | ×2.00 | VIP, Personal |

> Sala VIP: fotolii duble cu spațiu între ele (inspirat Cinema City).
> Biletul VIP (×1.00) este disponibil EXCLUSIV în Sala VIP. Premium-ul vine din multiplicatorul sălii.

### Configurarea prețurilor

Editați `data/preturi.csv` pentru a modifica prețurile **fără recompilare**:
```
Adult,1.00
Student,0.70
Elev,0.60
Pensionar,0.50
VIP,1.80
Personal,0.30
```
Prețul final = `pretBaza film × multiplicator tip bilet`.
