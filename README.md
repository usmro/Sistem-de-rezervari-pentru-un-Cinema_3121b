# Sistem de Rezervări pentru un Cinema

Aplicație C++17 orientată pe obiecte pentru gestionarea completă a rezervărilor într-un cinematograf, cu interfață consolă și server REST pentru frontend web.

**Autor:** Mandrescu Matei, sgr. 3121b

---

## Funcționalități principale

- Gestionarea entităților: `Film`, `Sala`, `Proiectie`, `Rezervare`, `User`, `Angajat`, `Voucher`, `ProdusConsumabil`
- **Design Patterns:** Singleton (`Cinematograf`), Factory (`BiletFactory`), Polimorfism (`Bilet`, `User → Angajat`)
- Sistem de autentificare cu roluri diferențiate: Admin / Client / Angajat
- Tipuri de sală: Standard, IMAX, VIP — cu configurație automată și bilete filtrate
- Sistem de vouchere cu validare pe zi și tip bilet
- Card de fidelitate — reducere 10% automată la rezervare
- Format audio per proiecție: Subtitrat / Dublat
- POS Bar pentru angajați/admin (vânzare rapidă cu bon fiscal)
- Pre-comandă snacks online — concept „skip the line" (legată de rezervare)
- Persistență completă în fișiere CSV
- Server REST (CROW) cu middleware CORS pentru frontend web
- Salvare automată la exit forțat (SIGINT/SIGTERM)

---

## Tehnologii

| Componentă | Tehnologie |
|---|---|
| Limbaj | C++17 |
| Build | CMake ≥ 3.15 |
| Server REST | CrowCpp 1.3.2 |
| Networking | Asio |
| Dependențe | vcpkg (Windows) / Homebrew (macOS) |
| Frontend | HTML + CSS + JavaScript pur |
| Version Control | Git & GitHub |

---

## Structura proiectului

```
├── include/
│   ├── Film.h, Sala.h, Proiectie.h, Rezervare.h
│   ├── Bilet.h, BiletFactory.h
│   ├── User.h, Angajat.h
│   ├── Voucher.h, FormatAudio.h, ProdusConsumabil.h
│   ├── ApiSerializer.h      # serializare JSON header-only
│   ├── CrowServer.h
│   ├── Cinematograf.h       # Singleton orchestrator
│   ├── StorageService.h     # persistenta CSV (cai relative!)
│   ├── ConsoleUI.h
│   └── Exceptii.h           # 10 exceptii custom
├── src/
│   ├── main.cpp             # entry point consolă
│   ├── main_crow.cpp        # entry point server REST
│   ├── CrowServer.cpp       # toate rutele REST + CORS middleware
│   ├── ConsoleUI.cpp
│   ├── Cinematograf.cpp
│   └── [Film|Sala|Proiectie|Rezervare|Bilet|User|Angajat|Voucher|ProdusConsumabil|StorageService].cpp
├── web/
│   ├── index.html           # welcome + login + register + program filme
│   ├── style.css, app.js    # stiluri și logică pentru index.html
│   ├── client.html          # flux rezervare 4 pași (self-contained)
│   └── admin.html           # dashboard admin (self-contained)
├── data/
│   ├── filme.csv, sali.csv, proiectii.csv
│   ├── rezervari.csv, useri.csv
│   ├── preturi.csv, vouchere.csv, snacks.csv
├── tests/
│   └── test_all.cpp
├── CMakeLists.txt
└── README.md
```

---

## Compilare și rulare

> **Important:** Rulează întotdeauna serverul din **rădăcina proiectului**, nu din `build/`.  
> `StorageService` folosește căi relative (`data/*.csv`) — altfel datele se scriu în locul greșit.

### macOS (Homebrew)

```bash
# 1. Instalează dependențele (o singură dată)
brew install cmake crow asio

# 2. Configurează CMake
cmake -B build -S . -DCMAKE_PREFIX_PATH="/opt/homebrew"

# 3. Compilează
cmake --build build --target CinemaServer -j 4   # serverul REST
cmake --build build --target CinemaApp -j 4      # aplicația consolă

# 4. Rulează (din rădăcina proiectului!)
./build/CinemaServer    # pornește pe http://localhost:8080
# sau
./build/CinemaApp
```

### Windows (MinGW + vcpkg)

```powershell
# 1. Instalează Crow prin vcpkg (o singură dată)
.\vcpkg\vcpkg install --triplet x64-windows

# 2. Configurează CMake
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE="vcpkg/scripts/buildsystems/vcpkg.cmake" `
  -DCMAKE_PREFIX_PATH="vcpkg/installed/x64-windows" `
  -G "MinGW Makefiles"

# 3. Compilează
cmake --build build --target CinemaServer -j 4
cmake --build build --target CinemaApp -j 4

# 4. Rulează (din rădăcina proiectului!)
.\build\CinemaServer.exe
```

### Linux

```bash
sudo apt install cmake libboost-dev
cmake -B build -S .
cmake --build build -j 4
./build/CinemaApp
```

---

## Frontend web

Pornește serverul CROW, apoi deschide `web/index.html` în browser.  
API-ul e disponibil la `http://localhost:8080/api/`.

### Pagini disponibile

| Pagină | Descriere |
|--------|-----------|
| `index.html` | Welcome, login, register, program filme |
| `client.html` | Flux rezervare complet în 4 pași |
| `admin.html` | Dashboard gestionare cinema |

### Flux rezervare client (`client.html`)

1. **Program filme** — grid cu gradienturi per gen, prețuri, genuri
2. **Selectare oră** — toggle date, proiecții grupate IMAX › VIP › Standard
3. **Selectare loc** — hartă vizuală a sălii, locuri ocupate marcate în timp real
4. **Snacks & Bar** — comandă opțională produse bar, ridici la ghișeul „Pre-Comenzi"

### Endpoint-uri REST

#### Publice

| Method | Endpoint | Descriere |
|--------|----------|-----------|
| POST | `/api/login` | Autentificare (`{"username","parola","tip"}`) |
| POST | `/api/register` | Înregistrare cont client (`{"username","parola"}`) |
| GET | `/api/filme` | Lista tuturor filmelor |
| GET | `/api/proiectii/viitoare` | Proiecții viitoare (cu film + sală embed) |
| GET | `/api/proiectii/vip` | Proiecții în săli VIP |
| GET | `/api/snacks` | Produse bar cu stoc |
| POST | `/api/verifica-voucher` | Validare voucher + returnare reducere % |
| POST | `/api/rezerva` | Creare rezervare |
| GET | `/api/proiectii/<id>/locuri` | Locuri ocupate pentru o proiecție |
| POST | `/api/comanda-snacks` | Pre-comandă snacks legată de rezervare |

#### Admin

| Method | Endpoint | Descriere |
|--------|----------|-----------|
| GET | `/api/admin/useri` | Lista toți userii |
| GET | `/api/admin/rezervari` | Lista toate rezervările |
| POST | `/api/admin/anuleaza` | Anulare rezervare |
| POST | `/api/admin/card-fidelitate` | Toggle card fidelitate user |
| POST | `/api/admin/filme` | Adaugă film nou |
| POST | `/api/admin/proiectii` | Adaugă proiecție nouă |

---

## Credențiale implicite

| Rol | Username | Parolă |
|-----|----------|--------|
| Admin | `admin` | `admin123` |

Admin-ul este creat automat la prima rulare dacă nu există.  
Clienții se pot înregistra din meniu sau din pagina web.

---

## Tipuri de sală

| Tip | Layout | Multiplicator | Bilete disponibile |
|-----|--------|---------------|--------------------|
| Standard | 10×15 = 150 locuri | ×1.00 | Adult, Student, Elev, Pensionar |
| IMAX | 8×12 = 96 locuri | ×1.50 | Adult, Student, Elev, Pensionar |
| VIP | 4×6 = 24 fotolii | ×2.00 | VIP, Personal, Adult |

**Formula preț:** `pretBazaFilm × multiplicatorBilet × multiplicatorSală`  
**Card fidelitate:** reducere suplimentară de 10% aplicată automat.  
**Voucher:** reducere procentuală aplicată pe prețul biletului.

---

## Fișiere CSV

| Fișier | Format |
|--------|--------|
| `filme.csv` | `titlu,durata,tip,gen,pretBaza` |
| `sali.csv` | `nume,tipSala,randuri,locuriPeRand` |
| `proiectii.csv` | `id,titluFilm,numeSala,dataOra,formatAudio` |
| `rezervari.csv` | `idRez,idProi,rand,loc,tipBilet,pret,username,anulat` |
| `useri.csv` | `username,parolaHash,rol,cardFidelitate` |
| `preturi.csv` | `tipBilet,multiplicator` |
| `vouchere.csv` | `cod,reducere%,ziuaInt,tipuriExcluse\|,activ` |
| `snacks.csv` | `nume,descriere,pret,stoc` |

Datele se salvează automat la ieșirea normală sau la SIGINT/SIGTERM.
