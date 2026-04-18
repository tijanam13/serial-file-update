# ASD — Serial File Update

A **file-based data processing** application written in **C**, implementing the classic problem of updating a serial (sorted sequential) file. Developed as an academic project for the course *Data Repository Programming (Programiranje repozitorijuma podataka)*, 4th year, Faculty of Organizational Sciences, University of Belgrade.

---

## Table of Contents

- [About the Project](#about-the-project)
- [Update Cases](#update-cases)
- [Project Structure](#project-structure)
- [Data Structures](#data-structures)
- [File Organization](#file-organization)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Menu Overview](#menu-overview)
- [Demo Data](#demo-data)
- [Acknowledgements](#acknowledgements)

---

## About the Project

The application solves the problem of updating a **serial (sorted sequential) file** — a master file of products sorted by product ID — using a transaction file that records daily inventory changes (stock in / stock out).

The core workflow is:

1. Raw transactions (chronological, unsorted) are recorded in `transakciona.dat`.
2. At end of day, a **summarized and sorted** transaction file is generated — one net record per product.
3. The master file (`maticna.dat`) is updated using the summary transaction file.
4. Reports and error logs are produced for each run.

The project covers **five update scenarios** of increasing complexity and is organized as a single Visual Studio solution with one project per scenario, plus a shared library.

---

## Update Cases

| # | Name | Description |
|---|------|-------------|
| 1 | **Basic Case** | All products exist; all quantities are valid. |
| 2 | **Insufficient Quantity** | Attempting to issue more than the available stock of an existing product. |
| 3 | **New Product** | Receiving a quantity of a product that does not yet exist in the master file — it gets added. |
| 4 | **Non-existent Product** | Attempting to issue a quantity of a product that does not exist in the master file — rejected with an error. |
| 5 | **Comprehensive Case** | All of the above scenarios combined. |

Each failed update is logged to a dedicated error report file instead of modifying the master file.

---

## Project Structure

```
ASD_solution/
├── ASD_solution.sln          # Visual Studio solution
├── Biblioteka/               # Shared library (used by all case projects)
│   ├── defs.h                # Type definitions and macros
│   ├── adts.h / adts.c       # Abstract data types: VektorProizvod, VektorTransakcija, Menu, file I/O
│   ├── misc.h / misc.c       # Business logic: case processors, CRUD operations
│   ├── util.h / util.c       # Utility functions
│   └── Biblioteka.vcxproj
├── Slucaj_1/                 # Update Case 1 — Basic
│   ├── main.c
│   ├── slucaj1.c
│   └── Slucaj_1.vcxproj
├── Slucaj_2/                 # Update Case 2 — Insufficient Quantity
├── Slucaj_3/                 # Update Case 3 — New Product
├── Slucaj_4/                 # Update Case 4 — Non-existent Product
└── Slucaj_5/                 # Update Case 5 — Comprehensive
```

At runtime, the application expects the following folder structure relative to the executable:

```
.\DATA\
    maticna.dat               # Current master file (binary)
    transakciona.dat          # Raw transaction file (binary)
    OLD\
        mat_<yymmdd>.dat      # Archived master file
        tran_<yymmdd>.dat     # Summarized & sorted transaction file
.\RPT\
    prom_<yymmdd>.rpt         # Change report (text)
    nov_pro_<yymmdd>.rpt      # New products report (text)
.\ERR\
    err_kol_<yymmdd>.rpt      # Error report: insufficient quantity (text)
    err_pro_<yymmdd>.rpt      # Error report: non-existent product (text)
.\DEMO\
    maticna.dat               # Demo master file
    SLUC_1\ ... SLUC_5\
        transakciona.dat      # Demo transaction file per case
```

---

## Data Structures

```c
// Product record (master file)
typedef struct {
    unsigned Id;
    char     Naziv[64];    // Product name
    unsigned Kolicina;     // Quantity
} PROIZVOD;

// Transaction record
typedef struct {
    unsigned Id;
    int      Promena;      // ULAZ = +1 (stock in), IZLAZ = -1 (stock out)
    unsigned Kolicina;     // Quantity
} TRANSAKCIJA;
```

Dynamic vectors (`VektorProizvod`, `VektorTransakcija`) are used for in-memory processing. The master file is a **binary sorted sequential file** (serial organization); the raw transaction file is an unsorted binary sequential file.

---

## File Organization

The project demonstrates the difference between:

- **Sequential (heap) file** — the raw transaction file, written chronologically.
- **Serial (sorted sequential) file** — the master file and the summarized transaction file, sorted by product ID.

The update algorithm performs a synchronized sequential scan of both sorted files (classic merge-update pattern), producing a new master file without random access.

---

## Getting Started

### Prerequisites

- Windows OS
- Visual Studio 2019 or later (with the Desktop development with C++ workload)
- C standard: C17

### Build

1. Open `ASD_solution/ASD_solution.sln` in Visual Studio.
2. Set the desired startup project (`Slucaj_1` through `Slucaj_5`).
3. Build the solution (**Build → Build Solution** or `Ctrl+Shift+B`).
4. The `Biblioteka` project is a static library shared by all case projects and will be compiled automatically.

### First Run

If `maticna.dat` or `transakciona.dat` are missing from `.\DATA\`, the application automatically copies the appropriate demo files from `.\DEMO\` before running.

---

## Usage

Run the compiled executable from within the correct working directory (the folder containing `DATA\`, `RPT\`, `ERR\`, and `DEMO\`).

```
asd.exe [-d[1|2|3|4|5]]
```

| Flag | Effect |
|------|--------|
| *(none)* | Launch with interactive menu |
| `-d1` | Load demo data for Case 1 |
| `-d2` | Load demo data for Case 2 |
| `-d3` | Load demo data for Case 3 |
| `-d4` | Load demo data for Case 4 |
| `-d5` | Load demo data for Case 5 |

---

## Menu Overview

```
GLAVNI MENI (Main Menu)
├── 1. Rad sa transakcionom datotekom (Transaction File)
│   ├── Create / Drop
│   ├── Insert
│   ├── Select All
│   └── Select by Id
├── 2. Rad sa matičnom datotekom (Master File)
│   ├── Create / Drop
│   ├── Insert / Delete
│   ├── Update All  ← core operation (full serial file update)
│   ├── Update by Id
│   ├── Select All
│   └── Select by Id
└── 3. Pomoć (Help)
    ├── About serial file updating
    ├── Demo (Cases 1–5)
    └── About
```

The **Update All** operation is the central function of the application. It performs the full serial file update and generates all applicable report and error files for the day.

---

## Demo Data

Each of the five update cases includes a ready-made demo dataset located in `.\DEMO\SLUC_<n>\transakciona.dat`. The shared master file is in `.\DEMO\maticna.dat`. The demo data covers all defined scenarios: successful updates, insufficient quantity errors, new product additions, and non-existent product errors.

---

## Acknowledgements

- **Institution:** Faculty of Organizational Sciences, University of Belgrade
- **Course:** Programiranje repozitorijuma podataka (*Data Repository Programming*), 4th year
- **Domain Engineer / Mentor:** Saša D. Lazarević (`slazar@fon.rs`)
- **Language:** C (C17)
- **IDE:** Microsoft Visual Studio
