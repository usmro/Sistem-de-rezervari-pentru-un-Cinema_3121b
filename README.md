# Sistem de Rezervări pentru un Cinema

Aplicație C++17 orientată pe obiecte pentru gestionarea completă a rezervărilor într-un cinematograf, cu interfață consolă și server REST (CrowCpp) pentru un frontend web modern.

**Autor:** Mandrescu Matei, grupa 3121b

---

## Funcționalități principale

### Domeniu & model
- Entități: `Film`, `Sala`, `Proiectie`, `Rezervare`, `User`, `Angajat`, `Voucher`, `ProdusConsumabil`, `Bilet`
- **Design patterns:** Singleton (`Cinematograf`), Factory (`BiletFactory`), Polimorfism (ierarhia `User → Angajat`, ierarhia de bilete)
- 10 excepții custom (`Exceptii.h`) pentru validări (loc ocupat, dată în trecut, autentificare eșuată etc.)

### Autentificare & roluri (3 roluri)
- **Client** — rezervă bilete, vede istoricul propriu, anulează rezervări
- **Angajat** — partea operațională: validare bilete, programul zilei, POS bar, rezervări, stoc snacks, comenzi VIP
- **Admin** — partea de management: filme, proiecții, săli, vouchere, useri, statistici

### Rezervări
- Rezervare multi-loc cu hartă vizuală a sălii (locuri ocupate în timp real)
- Tipuri de sală: Standard / IMAX / VIP, cu layout diferit (culoar central vs. perechi VIP)
- **Preț pe rând** (tier): rândurile din față ×0.85, mijloc ×1.00, spate ×1.20 (sălile VIP exceptate)
- Vouchere cu validare pe zi și pe tip de bilet
- Card de fidelitate — reducere automată 10%
- Extras premium în sala VIP (șampanie, platouri etc.), comandate la rezervare și persistate
- **Bilet cu cod QR scanabil** (conține ID-ul rezervării) + cod de bare, în confirmarea rezervării

### Cont client
- Istoric rezervări cu **filtre** (status, perioadă viitoare/trecut, căutare film) și **sortare**
- Anulare rezervare direct de către client (locul se eliberează)
- Rezumat: bilete active + total cheltuit

### Program & căutare
- Pagina „Program" cu **căutare** după titlu și **filtre** după gen, format (2D/3D) și zi

### Panou angajat (operațional)
- **Validare bilet** după cod (valid / anulat / inexistent) — scanezi QR-ul sau introduci ID-ul
- **Programul zilei** cu grad de ocupare per proiecție
- **POS Bar** — vânzare snacks la casă (bon cu TVA)
- **Rezervări** — lista biletelor + căutare/filtru/sortare + anulare + export CSV
- **Stoc snacks** — reaprovizionare
- **Comenzi VIP premium**

### Panou admin (management)
- CRUD filme și proiecții (adăugare / **editare** / **ștergere**, cu protecție: nu se șterge/editează o proiecție cu rezervări active)
- **Grad de ocupare** per proiecție (bară + procent)
- Statistici: **venituri pe film** și **încasări pe perioadă** (săptămână / lună)
- Gestionare vouchere (adăugare / activare / ștergere)
- Tabel săli încărcat dinamic; **Clienți** și **Angajați** afișați separat
- Export CSV al rezervărilor (din panoul angajat)

### Tehnic
- Persistență în fișiere CSV — **incrementală**: fiecare modificare se salvează imediat
- **Thread-safety**: mutex global (`std::lock_guard`) pe toate rutele care modifică starea → previne dubla-rezervare la cereri concurente
- Server REST (CrowCpp) cu middleware CORS

---

## Tehnologii

| Componentă | Tehnologie |
|---|---|
| Limbaj | C++17 |
| Build | CMake ≥ 3.15 |
| Server REST | CrowCpp |
| Networking | Asio |
| Dependențe | vcpkg (Windows) / Homebrew (macOS) |
| Frontend | HTML + CSS + JavaScript pur |
| Persistență | fișiere CSV (căi relative) |

---

## Structura proiectului

```
├── include/                 # headere (clasele de domeniu + servicii)
│   ├── Film.h, Sala.h, Proiectie.h, Rezervare.h
│   ├── Bilet.h, BiletFactory.h
│   ├── User.h, Angajat.h
│   ├── Voucher.h, FormatAudio.h, ProdusConsumabil.h
│   ├── ApiSerializer.h      # serializare JSON header-only
│   ├── Cinematograf.h       # Singleton orchestrator
│   ├── CrowServer.h
│   ├── StorageService.h     # persistență CSV (căi relative!)
│   ├── ConsoleUI.h
│   └── Exceptii.h           # excepții custom
├── src/
│   ├── main.cpp             # entry point consolă (CinemaApp)
│   ├── main_crow.cpp        # entry point server REST (CinemaServer)
│   ├── CrowServer.cpp       # toate rutele REST + CORS + lock-uri
│   ├── ConsoleUI.cpp
│   ├── Cinematograf.cpp
│   └── [Film|Sala|Proiectie|Rezervare|Bilet|User|Angajat|Voucher|ProdusConsumabil|StorageService].cpp
├── web/
│   ├── index.html           # welcome + login (3 roluri) + register + program + cont client
│   ├── style.css, app.js    # stiluri și logică pentru index.html
│   ├── client.html          # flux rezervare în 4 pași (self-contained)
│   ├── admin.html           # dashboard admin (self-contained)
│   └── angajat.html         # panou angajat operațional (validare, program, POS, rezervări, stoc, VIP)
├── data/                    # fișiere CSV (date persistate)
│   ├── filme.csv, sali.csv, proiectii.csv
│   ├── rezervari.csv, useri.csv, preturi.csv
│   ├── vouchere.csv, snacks.csv, vip_extras.csv
├── CMakeLists.txt
└── README.md
```

---

## Compilare și rulare

> **Important:** Rulează întotdeauna serverul din **rădăcina proiectului**, nu din `build/`.
> `StorageService` folosește căi relative (`data/*.csv`).

### macOS (Homebrew)

```bash
brew install cmake crow asio
cmake -B build -S . -DCMAKE_PREFIX_PATH="/opt/homebrew"
cmake --build build --target CinemaServer -j 4   # serverul REST
cmake --build build --target CinemaApp -j 4      # aplicația consolă
./build/CinemaServer    # pornește pe http://localhost:8080
```

### Windows (MinGW + vcpkg)

```powershell
.\vcpkg\vcpkg install --triplet x64-windows
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE="vcpkg/scripts/buildsystems/vcpkg.cmake" `
  -DCMAKE_PREFIX_PATH="vcpkg/installed/x64-windows" `
  -G "MinGW Makefiles"
cmake --build build --target CinemaServer -j 4
.\build\CinemaServer.exe
```

### Linux

```bash
sudo apt install cmake libboost-dev
cmake -B build -S .
cmake --build build -j 4
./build/CinemaServer
```

---

## Frontend web

Aplicația e **una singură**: `CinemaServer` (Crow) servește **și** paginile din `web/`, **și** API-ul.
Pornește serverul și deschide în browser **`http://localhost:8080`** — atât. Nu mai e nevoie de un server static separat, iar API-ul e pe `http://localhost:8080/api/`.

| Pagină | Rol | Descriere |
|--------|-----|-----------|
| `index.html` | toți | Welcome, login (Client/Angajat/Admin), register, program filme, contul meu |
| `client.html` | Client | Flux rezervare complet în 4 pași |
| `angajat.html` | Angajat | Dashboard operațional: validare bilet (QR/cod), program, POS bar, rezervări, stoc snacks, comenzi VIP |
| `admin.html` | Admin | Management: filme, săli, proiecții, statistici, vouchere, clienți & angajați |

### Flux rezervare client (`client.html`)
1. **Program filme** — grid cu prețuri și genuri
2. **Selectare oră** — proiecții grupate pe săli
3. **Selectare loc** — hartă vizuală, locuri ocupate marcate în timp real, preț pe rând
4. **Snacks & Bar** — comandă opțională + confirmare cu detalii complete înainte de plată

---

## Endpoint-uri REST

### Publice / Client
| Method | Endpoint | Descriere |
|--------|----------|-----------|
| POST | `/api/login` | Autentificare (`{username, parola, tip}`; tip = client/angajat/admin) |
| POST | `/api/register` | Înregistrare cont client |
| GET | `/api/filme` | Lista filmelor |
| GET | `/api/sali` | Lista sălilor |
| GET | `/api/proiectii/viitoare` | Proiecții viitoare (film + sală embed) |
| GET | `/api/proiectii/vip` | Proiecții în săli VIP |
| GET | `/api/proiectii/<id>/locuri` | Locuri ocupate pentru o proiecție |
| GET | `/api/snacks` | Produse bar cu stoc |
| POST | `/api/verifica-voucher` | Validare voucher + reducere % |
| POST | `/api/rezerva` | Creare rezervare |
| POST | `/api/comanda-snacks` | Pre-comandă snacks |
| POST | `/api/vip-extras` | Extras premium VIP (persistate) |
| GET | `/api/rezervarile-mele?username=` | Rezervările unui client |
| POST | `/api/anuleaza` | Anulare rezervare de către client |

### Angajat
| Method | Endpoint | Descriere |
|--------|----------|-----------|
| GET | `/api/rezervare/<id>` | Verificare bilet (valid / anulat / inexistent) |

### Admin
| Method | Endpoint | Descriere |
|--------|----------|-----------|
| GET | `/api/admin/useri` | Lista userilor |
| GET | `/api/admin/rezervari` | Lista rezervărilor (cu `proiectieId`) |
| GET | `/api/admin/vouchere` | Lista voucherelor |
| GET | `/api/admin/vip-extras` | Comenzile VIP premium |
| POST | `/api/admin/anuleaza` | Anulare rezervare |
| POST | `/api/admin/card-fidelitate` | Toggle card fidelitate |
| POST | `/api/admin/filme` | Adaugă film |
| DELETE | `/api/admin/filme/<titlu>` | Șterge film |
| POST | `/api/admin/proiectii` | Adaugă proiecție (anti-conflict sală/oră) |
| POST | `/api/admin/proiectii/edit` | Editează proiecție (blocat dacă are rezervări active) |
| DELETE | `/api/admin/proiectii/<id>` | Șterge proiecție (blocat dacă are rezervări active) |
| POST | `/api/admin/vouchere/add` · `/toggle` · DELETE `/<cod>` | Gestionare vouchere |
| POST | `/api/admin/snacks/refill` | Reaprovizionare stoc |

---

## Credențiale demo

| Rol | Username | Parolă |
|-----|----------|--------|
| Admin | `admin` | `admin123` |
| Angajat | `angajat1`, `angajat2` | `parola123` |
| Client | `ana`, `andrei`, `ioana`, `vlad` | `parola123` |

Fiecare client demo are rezervări variate (viitoare + trecute, săli și ore diferite).
Admin-ul este creat automat la prima rulare dacă nu există. Clienți noi se pot înregistra din pagina web.

---

## Tipuri de sală și prețuri

| Tip | Layout | Multiplicator sală | Bilete |
|-----|--------|--------------------|--------|
| Standard | culoar central (ex. 10×15) | ×1.00 | Adult, Student, Copil, Pensionar |
| IMAX | culoar central (ex. 8×12) | ×1.50 | Adult, Student, Copil, Pensionar |
| VIP | perechi de 2 (ex. 4×6) | ×2.00 | VIP, Adult |

**Formula preț final:**
```
pretFinal = pretBazaFilm × multiplicatorBilet × multiplicatorSală × multiplicatorRând
```
- **Multiplicator rând** (doar Standard/IMAX): față (≤30%) ×0.85, mijloc (≤70%) ×1.00, spate (>70%) ×1.20
- **Card fidelitate:** −10% suplimentar
- **Voucher:** reducere procentuală pe prețul biletului (validată pe zi și tip)

---

## Fișiere CSV

| Fișier | Format |
|--------|--------|
| `filme.csv` | `titlu,durata,tip,gen,pretBaza` |
| `sali.csv` | `nume,tipSala,randuri,locuriPeRand` |
| `proiectii.csv` | `id,titluFilm,numeSala,dataOra,formatAudio` |
| `rezervari.csv` | `idRez,idProi,rand,loc,tipBilet,pret,username,anulat` |
| `useri.csv` | `username,parolaHash,rol,cardFidelitate` (Angajat: `...,Angajat,program,sala`) |
| `preturi.csv` | `tipBilet,multiplicator` |
| `vouchere.csv` | `cod,reducere%,ziuaInt,tipuriExcluse\|,activ` |
| `snacks.csv` | `nume,descriere,pret,stoc` |
| `vip_extras.csv` | `idRezervare⇥extrasJSON` (delimitat prin TAB) |

Datele se salvează **incremental** — fiecare operație (rezervare, anulare, acțiune admin) scrie imediat în CSV.
Parolele sunt hash-uite (djb2) — suficient pentru scop didactic, nu pentru producție.
