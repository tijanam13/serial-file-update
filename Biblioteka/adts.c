#define _CRT_SECURE_NO_WARNINGS
#include "adts.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "util.h"
#include "misc.h"


void vp_init(VektorProizvod* v, size_t initialCapacity) {
	v->items = NULL;
	v->size = 0;
	v->capacity = 0;
	if (initialCapacity > 0) {
		v->items = (PROIZVOD*)malloc(sizeof(PROIZVOD) * initialCapacity);
		if (v->items) v->capacity = initialCapacity;
	}
}

void vp_free(VektorProizvod* v) {
	free(v->items);
	v->items = NULL;
	v->size = 0;
	v->capacity = 0;
}

static int vp_ensure_capacity(VektorProizvod* v, size_t minCap) {
	if (v->capacity >= minCap) return 1;
	size_t newCap = v->capacity ? v->capacity * 2 : 8;
	if (newCap < minCap) newCap = minCap;
	PROIZVOD* tmp = (PROIZVOD*)realloc(v->items, sizeof(PROIZVOD) * newCap);
	if (!tmp) return 0;
	v->items = tmp;
	v->capacity = newCap;
	return 1;
}

int vp_push(VektorProizvod* v, const PROIZVOD* p) {
	if (!vp_ensure_capacity(v, v->size + 1)) return 0;
	v->items[v->size++] = *p;
	return 1;
}

int vp_insert_at(VektorProizvod* v, size_t index, const PROIZVOD* p) {
	if (index > v->size) return 0;
	if (!vp_ensure_capacity(v, v->size + 1)) return 0;
	for (size_t i = v->size; i > index; --i) v->items[i] = v->items[i - 1];
	v->items[index] = *p;
	v->size++;
	return 1;
}

int vp_find_by_id(const VektorProizvod* v, unsigned id) {
	for (size_t i = 0; i < v->size; ++i) {
		if (v->items[i].Id == id) return (int)i;
	}
	return -1;
}

PROIZVOD* vp_get(VektorProizvod* v, size_t index) {
	if (index >= v->size) return NULL;
	return &v->items[index];
}

int vp_remove_by_index(VektorProizvod* v, size_t index) {
	if (index >= v->size) return 0;
	for (size_t i = index; i + 1 < v->size; ++i) v->items[i] = v->items[i + 1];
	v->size--;
	return 1;
}

void vt_init(VektorTransakcija* v, size_t initialCapacity) {
	v->items = NULL;
	v->size = 0;
	v->capacity = 0;
	if (initialCapacity > 0) {
		v->items = (TRANSAKCIJA*)malloc(sizeof(TRANSAKCIJA) * initialCapacity);
		if (v->items) v->capacity = initialCapacity;
	}
}

void vt_free(VektorTransakcija* v) {
	free(v->items);
	v->items = NULL;
	v->size = 0;
	v->capacity = 0;
}

static int vt_ensure_capacity(VektorTransakcija* v, size_t minCap) {
	if (v->capacity >= minCap) return 1;
	size_t newCap = v->capacity ? v->capacity * 2 : 8;
	if (newCap < minCap) newCap = minCap;
	TRANSAKCIJA* tmp = (TRANSAKCIJA*)realloc(v->items, sizeof(TRANSAKCIJA) * newCap);
	if (!tmp) return 0;
	v->items = tmp;
	v->capacity = newCap;
	return 1;
}

int vt_push(VektorTransakcija* v, const TRANSAKCIJA* t) {
	if (!vt_ensure_capacity(v, v->size + 1)) return 0;
	v->items[v->size++] = *t;
	return 1;
}

int vt_find_by_id(const VektorTransakcija* v, unsigned id) {
	for (size_t i = 0; i < v->size; ++i) {
		if (v->items[i].Id == id) return (int)i;
	}
	return -1;
}

TRANSAKCIJA* vt_get(VektorTransakcija* v, size_t index) {
	if (index >= v->size) return NULL;
	return &v->items[index];
}

int vt_remove_by_index(VektorTransakcija* v, size_t index) {
	if (index >= v->size) return 0;
	for (size_t i = index; i + 1 < v->size; ++i) v->items[i] = v->items[i + 1];
	v->size--;
	return 1;
}

int dat_ucitaj_maticnu(const char* path, VektorProizvod* out) {
	FILE* f = fopen(path, "rb");
	if (!f) return 0;
	PROIZVOD tmp;
	while (fread(&tmp, sizeof(PROIZVOD), 1, f) == 1) {
		if (!vp_push(out, &tmp)) { fclose(f); return 0; }
	}
	fclose(f);
	return 1;
}

int dat_sacuvaj_maticnu(const char* path, const VektorProizvod* v) {
	FILE* f = fopen(path, "wb");
	if (!f) return 0;
	if (v->size > 0) {
		if (fwrite(v->items, sizeof(PROIZVOD), v->size, f) != v->size) {
			fclose(f);
			return 0;
		}
	}
	fclose(f);
	return 1;
}

int dat_ucitaj_transakcije(const char* path, VektorTransakcija* out) {
	FILE* f = fopen(path, "rb");
	if (!f) return 0;
	TRANSAKCIJA tmp;
	while (fread(&tmp, sizeof(TRANSAKCIJA), 1, f) == 1) {
		if (!vt_push(out, &tmp)) { fclose(f); return 0; }
	}
	fclose(f);
	return 1;
}


static char* dupstr(const char* s) {
	if (!s) return NULL;
	size_t len = strlen(s) + 1;
	char* d = (char*)malloc(len);
	if (d) memcpy(d, s, len);
	return d;
}

void menu_init(Menu* m, const char* title) {
	if (!m) return;
	m->title = dupstr(title ? title : "");
	m->items = NULL;
	m->count = 0;
	m->capacity = 0;
}

void menu_free(Menu* m) {
	if (!m) return;
	if (m->title) free(m->title);
	if (m->items) {
		for (size_t i = 0; i < m->count; ++i) {
			if (m->items[i].text) free(m->items[i].text);
		}
		free(m->items);
	}
	m->title = NULL;
	m->items = NULL;
	m->count = 0;
	m->capacity = 0;
}

static void akcija_nije_implementirana(void) {
	printf("\nINFO: Funkcija azuriranja nije implementirana u ovom projektu.\n");
	printf("Pritisnite ENTER za povratak...");
	util_clear_line();
}

static int menu_ensure_capacity(Menu* m, size_t minCap) {
	if (m->capacity >= minCap) return 1;
	size_t oldCap = m->capacity;
	size_t newCap = m->capacity ? m->capacity * 2 : 8;
	if (newCap < minCap) newCap = minCap;

	MenuItem* tmp = (MenuItem*)realloc(m->items, sizeof(MenuItem) * newCap);
	if (!tmp) return 0;

	for (size_t i = oldCap; i < newCap; i++) {
		tmp[i].text = NULL;
		tmp[i].action = NULL;
	}

	m->items = tmp;
	m->capacity = newCap;
	return 1;
}

int menu_add_item(Menu* m, const char* label, MenuAction action) {
	if (!m || !label) return 0;
	if (!menu_ensure_capacity(m, m->count + 1)) return 0;
	m->items[m->count].text = dupstr(label);
	m->items[m->count].action = action;
	m->count++;
	return 1;
}

static void clear_stdin(void) {
	int c;
	while ((c = getchar()) != EOF && c != '\n');
}

void menu_run(Menu* m) {
	if (!m) return;
	int choice = -1;

	while (1) {
		printf("\n========================================\n");
		printf("%s\n", m->title ? m->title : "");
		printf("========================================\n");

		for (size_t i = 0; i < m->count; ++i) {
			printf("%zu. %s\n", i + 1, m->items[i].text ? m->items[i].text : "");
		}
		printf("0. Povratak\n\n");
		printf("Izbor: ");

		if (scanf("%d", &choice) != 1) {
			clear_stdin();
			printf("Pogresan unos.\n");
			continue;
		}

		clear_stdin();

		if (choice == 0) return;
		if (choice < 0 || (size_t)choice > m->count) {
			printf("Pogresan unos.\n");
			continue;
		}

		MenuAction act = m->items[choice - 1].action;
		if (act) act();
	}
}


static MenuAction g_updateAllAction = NULL;

static void run_transakcija_menu(void) {
	Menu m;
	menu_init(&m, "MENI Transakciona datoteka");

	menu_add_item(&m, "Create ::= Kreiranje nove transakcione datoteke", kreirajTransakcionuDatoteku);
	menu_add_item(&m, "Drop ::= Unistavanje postojece transakcione datoteke", obrisiTransakcionuDatoteku);
	menu_add_item(&m, "Insert ::= Dodavanje nove transakcije", ubaciTransakciju);
	menu_add_item(&m, "Select All ::= Prikazivanje svih transakcija", prikaziSveTransakcije);
	menu_add_item(&m, "Select Id ::= Prikazivanje svih transakcija jednog proizvoda", prikaziTransakcijeJednogProizvoda);

	menu_run(&m);
	menu_free(&m);
}

static void run_maticna_menu(void) {
	Menu m;
	menu_init(&m, "MENI Maticna datoteka");

	menu_add_item(&m, "Create ::= Kreiranje nove maticne datoteke", kreirajMaticnuDatoteku);
	menu_add_item(&m, "Drop ::= Unistavanje postojece maticne datoteke", obrisiMaticnuDatoteku);
	menu_add_item(&m, "Insert ::= Dodavanje novog proizvoda", ubaciProizvod);
	menu_add_item(&m, "Delete ::= Brisanje postojeceg proizvoda", obrisiProizvod);
	menu_add_item(&m, "Update All ::= Azuriranje maticne datoteke upotrebom transakcione datoteke", g_updateAllAction ? g_updateAllAction : akcija_nije_implementirana);
	menu_add_item(&m, "Update Id ::= Azuriranje jednog proizvoda", azurirajJedanProizvod);
	menu_add_item(&m, "Select All ::= Prikazivanje svih proizvoda", prikaziSveProizvode);
	menu_add_item(&m, "Select Id ::= Prikazivanje jednog proizvoda", prikaziJedanProizvod);

	menu_run(&m);
	menu_free(&m);
}
static void prikaziDemoTekst(int slucaj) {
	char datum[7];
	formirajDatum(datum);

	const char* linija_mat = "+----+--------------+----------+";
	const char* linija_tra = "+----+----------+----------+";
	const char* linija_rpt = "+----+----------+--------------+-----+----------+----------+";
	const char* linija_err = "+----+----------+--------------+-----+----------+--------------------------------+";

	switch (slucaj) {
	case 1:
		system("cls");
		printf("1. OSNOVNI SLUCAJ - TEKSTUALNI OPIS\n");
		printf("=================================================================\n");
		printf("Ovaj slucaj je najjednostavniji. Azurira se proizvod koji postoji\n");
		printf("u maticnoj datoteci, a kolicine su uvek odgovarajuce.\n\n");
		printf("Scenario:\n");
		printf("(a) Primljena kolicina se dodaje na trenutnu\n");
		printf("(b) Izdata kolicina je manja od trenutne i oduzima se\n");
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 1: STARA MATICNA DATOTEKA (maticna.dat)\n");
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n", linija_mat, linija_mat);
		printf("| 20 | Pro_20       |      100 |\n| 30 | Pro_30       |      150 |\n| 40 | Pro_40       |      200 |\n| 50 | Pro_50       |      250 |\n| 60 | Pro_60       |      300 |\n| 70 | Pro_70       |      350 |\n| 80 | Pro_80       |      400 |\n| 90 | Pro_90       |      450 |\n%s\n", linija_mat);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 1: TRANSAKCIONA DATOTEKA (transakciona.dat - hronoloski)\n");
		printf("%s\n| Id | Promena  | Kolicina |\n%s\n"
			"| 60 | ULAZ     |      150 |\n"
			"| 40 | ULAZ     |       80 |\n"
			"| 60 | ULAZ     |       50 |\n"
			"| 60 | IZLAZ    |      100 |\n"
			"| 40 | IZLAZ    |       50 |\n"
			"| 60 | IZLAZ    |      150 |\n"
			"| 60 | ULAZ     |      200 |\n"
			"| 40 | IZLAZ    |       80 |\n"
			"| 60 | ULAZ     |       20 |\n"
			"| 60 | IZLAZ    |      120 |\n"
			"| 40 | ULAZ     |      100 |\n"
			"| 60 | IZLAZ    |      100 |\n"
			"| 70 | ULAZ     |       50 |\n"
			"| 70 | IZLAZ    |       50 |\n"
			"%s\n",
			linija_tra, linija_tra, linija_tra);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 1: SUMARNA I SORTIRANA DATOTEKA PROMENA (tran_%s.dat)\n", datum);
		printf("%s\n| Id | Promena  | Kolicina |\n%s\n", linija_tra, linija_tra);
		printf("| 40 | ULAZ     |       50 |\n| 60 | IZLAZ    |       50 |\n| 70 | ULAZ     |        0 |\n%s\n", linija_tra);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 1: NOVA MATICNA DATOTEKA (maticna.dat)\n");
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n"
			"| 20 | Pro_20       |      100 |\n"
			"| 30 | Pro_30       |      150 |\n"
			"| 40 | Pro_40       |      250 |\n"
			"| 50 | Pro_50       |      250 |\n"
			"| 60 | Pro_60       |      250 |\n"
			"| 70 | Pro_70       |      350 |\n"
			"| 80 | Pro_80       |      400 |\n"
			"| 90 | Pro_90       |      450 |\n"
			"%s\n",
			linija_mat, linija_mat, linija_mat);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();
		system("cls");
		printf("DEMO 1: IZVESTAJ O PROMENAMA (prom_%s.rpt)\n", datum);
		printf("%s\n", linija_rpt);
		printf("|          Proizvod            |     Promena    |   Nova   |\n");
		printf("%s\n", linija_rpt);
		printf("| Id | Kolicina |    Naziv     | Tip | Kolicina | kolicina |\n");
		printf("%s\n", linija_rpt);
		printf("| 40 |      200 | Pro_40       |  +  |       50 |      250 |\n"
			"| 60 |      300 | Pro_60       |  -  |       50 |      250 |\n"
			"| 70 |      350 | Pro_70       |  +  |        0 |      350 |\n"
			"%s\n",
			linija_rpt);
		printf("\n[Pritisnite ENTER za povratak u meni]");
		util_clear_line();
		break;

	case 2:
		system("cls");
		printf("2. NEPOSTOJECA KOLICINA - OPIS\n");
		printf("=================================================================\n");
		printf("Azurira se postojeci proizvod, ali se pokusava izdavanje vece\n");
		printf("kolicine od raspolozive.\n\n");
		printf("Scenario:\n");
		printf("Izdata kolicina > Stanje = GRESKA\n");
		printf("Operacija se odbija, formira se izvestaj o greskama.\n");
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 1: STARA MATICNA DATOTEKA (maticna.dat)\n");
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n", linija_mat, linija_mat);
		printf("| 20 | Pro_20       |      100 |\n| 30 | Pro_30       |      150 |\n| 40 | Pro_40       |      200 |\n| 50 | Pro_50       |      250 |\n| 60 | Pro_60       |      300 |\n| 70 | Pro_70       |      350 |\n| 80 | Pro_80       |      400 |\n| 90 | Pro_90       |      450 |\n%s\n", linija_mat);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 2: TRANSAKCIONA DATOTEKA (transakciona.dat - hronoloski)\n");
		printf("%s\n| Id | Promena  | Kolicina |\n%s\n"
			"| 80 | IZLAZ    |       50 |\n"
			"| 70 | IZLAZ    |      400 |\n"
			"| 80 | ULAZ     |      100 |\n"
			"| 90 | IZLAZ    |      800 |\n"
			"| 60 | IZLAZ    |      500 |\n"
			"| 20 | IZLAZ    |      500 |\n"
			"| 30 | ULAZ     |      100 |\n"
			"| 20 | IZLAZ    |      300 |\n"
			"| 60 | IZLAZ    |      300 |\n"
			"| 40 | ULAZ     |      150 |\n"
			"| 50 | IZLAZ    |      200 |\n"
			"| 40 | IZLAZ    |      100 |\n"
			"| 50 | IZLAZ    |      300 |\n"
			"| 50 | IZLAZ    |      300 |\n"
			"| 30 | IZLAZ    |       50 |\n"
			"| 90 | ULAZ     |      800 |\n"
			"| 70 | ULAZ     |      200 |\n"
			"%s\n",
			linija_tra, linija_tra, linija_tra);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 2: SUMARNA I SORTIRANA DATOTEKA PROMENA (tran_%s.dat)\n", datum);
		printf("%s\n| Id | Promena  | Kolicina |\n%s\n"
			"| 20 | IZLAZ    |      800 |\n"
			"| 30 | ULAZ     |       50 |\n"
			"| 40 | ULAZ     |       50 |\n"
			"| 50 | IZLAZ    |      800 |\n"
			"| 60 | IZLAZ    |      800 |\n"
			"| 70 | IZLAZ    |      200 |\n"
			"| 80 | ULAZ     |       50 |\n"
			"| 90 | ULAZ     |        0 |\n"
			"%s\n",
			linija_tra, linija_tra, linija_tra);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 2: NOVA MATICNA DATOTEKA (maticna.dat)\n");
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n"
			"| 20 | Pro_20       |      100 |\n"
			"| 30 | Pro_30       |      200 |\n"
			"| 40 | Pro_40       |      250 |\n"
			"| 50 | Pro_50       |      250 |\n"
			"| 60 | Pro_60       |      300 |\n"
			"| 70 | Pro_70       |      150 |\n"
			"| 80 | Pro_80       |      450 |\n"
			"| 90 | Pro_90       |      450 |\n"
			"%s\n",
			linija_mat, linija_mat, linija_mat);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 2: IZVESTAJ O PROMENAMA (prom_%s.rpt)\n", datum);
		printf("%s\n", linija_rpt);
		printf("|          Proizvod            |     Promena    |   Nova   |\n");
		printf("%s\n", linija_rpt);
		printf("| Id | Kolicina |    Naziv     | Tip | Kolicina | kolicina |\n");
		printf("%s\n", linija_rpt);
		printf("| 30 |      150 | Pro_30       |  +  |       50 |      200 |\n"
			"| 40 |      200 | Pro_40       |  +  |       50 |      250 |\n"
			"| 70 |      350 | Pro_70       |  -  |      200 |      150 |\n"
			"| 80 |      400 | Pro_80       |  +  |       50 |      450 |\n"
			"| 90 |      450 | Pro_90       |  +  |        0 |      450 |\n"
			"%s\n",
			linija_rpt);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 2: IZVESTAJ O GRESKAMA (err_kol_%s.rpt)\n", datum);
		printf("%s\n| Id | Kolicina | Naziv        | Tip | Kolicina |            Greska              |\n%s\n", linija_err, linija_err);
		printf("| 20 |      100 | Pro_20       |  -  |      800 | Nepostojeca kolicina proizvoda |\n");
		printf("| 50 |      250 | Pro_50       |  -  |      800 | Nepostojeca kolicina proizvoda |\n");
		printf("| 60 |      300 | Pro_60       |  -  |      800 | Nepostojeca kolicina proizvoda |\n%s\n", linija_err);
		printf("\n[Pritisnite ENTER za povratak u meni]");
		util_clear_line();
		break;

	case 3:
		system("cls");
		printf("3. NOV PROIZVOD - OPIS\n");
		printf("=================================================================\n");
		printf("Odvija se azuriranje, ali se prima kolicina proizvoda koji ne postoji.\n\n");
		printf("Scenario:\n");
		printf("Nepostojeci proizvod + ULAZ = DODAVANJE NOVOG\n");
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO: STARA MATICNA DATOTEKA (maticna.dat)\n");
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n"
			"| 20 | Pro_20       |      100 |\n"
			"| 30 | Pro_30       |      150 |\n"
			"| 40 | Pro_40       |      200 |\n"
			"| 50 | Pro_50       |      250 |\n"
			"| 60 | Pro_60       |      300 |\n"
			"| 70 | Pro_70       |      350 |\n"
			"| 80 | Pro_80       |      400 |\n"
			"| 90 | Pro_90       |      450 |\n"
			"%s\n",
			linija_mat, linija_mat, linija_mat);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 3: TRANSAKCIONA DATOTEKA (transakciona.dat - hronoloski)\n");
		printf("%s\n| Id | Promena  | Kolicina |\n%s\n"
			"| 55 | ULAZ     |      205 |\n"
			"| 95 | ULAZ     |      295 |\n"
			"| 55 | ULAZ     |       50 |\n"
			"| 45 | ULAZ     |      245 |\n"
			"| 95 | IZLAZ    |       15 |\n"
			"| 15 | ULAZ     |      230 |\n"
			"| 10 | ULAZ     |      190 |\n"
			"| 20 | IZLAZ    |      100 |\n"
			"| 30 | IZLAZ    |       50 |\n"
			"| 20 | ULAZ     |       50 |\n"
			"| 35 | ULAZ     |      285 |\n"
			"| 36 | ULAZ     |      236 |\n"
			"| 35 | IZLAZ    |       50 |\n"
			"| 37 | ULAZ     |      240 |\n"
			"| 37 | IZLAZ    |        3 |\n"
			"| 70 | ULAZ     |      100 |\n"
			"| 10 | IZLAZ    |       20 |\n"
			"| 10 | ULAZ     |       40 |\n"
			"| 15 | IZLAZ    |       15 |\n"
			"| 95 | ULAZ     |       15 |\n"
			"%s\n",
			linija_tra, linija_tra, linija_tra);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 3: SUMARNA I SORTIRANA DATOTEKA PROMENA (tran_%s.dat)\n", datum);
		printf("%s\n| Id | Promena  | Kolicina |\n%s\n"
			"| 10 | ULAZ     |      210 |\n"
			"| 15 | ULAZ     |      215 |\n"
			"| 20 | IZLAZ    |      100 |\n"
			"| 30 | IZLAZ    |       50 |\n"
			"| 35 | ULAZ     |      235 |\n"
			"| 36 | ULAZ     |      236 |\n"
			"| 37 | ULAZ     |      237 |\n"
			"| 45 | ULAZ     |      245 |\n"
			"| 55 | ULAZ     |      255 |\n"
			"| 70 | ULAZ     |      100 |\n"
			"| 90 | ULAZ     |      295 |\n"
			"%s\n",
			linija_tra, linija_tra, linija_tra);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 3: NOVA MATICNA DATOTEKA (maticna.dat)\n");
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n"
			"| 10 | Pro_10       |      210 |\n"
			"| 15 | Pro_15       |      215 |\n"
			"| 20 | Pro_20       |       50 |\n"
			"| 30 | Pro_30       |      100 |\n"
			"| 35 | Pro_35       |      235 |\n"
			"| 36 | Pro_36       |      236 |\n"
			"| 37 | Pro_37       |      237 |\n"
			"| 40 | Pro_40       |      200 |\n"
			"| 45 | Pro_45       |      245 |\n"
			"| 50 | Pro_50       |      250 |\n"
			"| 55 | Pro_55       |      255 |\n"
			"| 60 | Pro_60       |      300 |\n"
			"| 70 | Pro_70       |      450 |\n"
			"| 80 | Pro_80       |      400 |\n"
			"| 90 | Pro_90       |      450 |\n"
			"| 95 | Pro_95       |      295 |\n"
			"%s\n",
			linija_mat, linija_mat, linija_mat);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 3: IZVESTAJ O PROMENAMA (prom_%s.rpt)\n", datum);
		printf("%s\n", linija_rpt);
		printf("|          Proizvod            |    Promena     |   Nova   |\n");
		printf("%s\n", linija_rpt);
		printf("| Id | Kolicina |    Naziv     | Tip | Kolicina | kolicina |\n");
		printf("%s\n", linija_rpt);
		printf("| 20 |      100 | Pro_20       |  -  |       50 |       50 |\n"
			"| 30 |      150 | Pro_30       |  -  |       50 |      100 |\n"
			"| 70 |      350 | Pro_70       |  +  |      100 |      450 |\n"
			"%s\n",
			linija_rpt);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 3: IZVESTAJ O NOVIM PROIZVODIMA (nov_pro_%s.rpt)\n", datum);
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n"
			"| 10 | Pro_10       |      210 |\n"
			"| 15 | Pro_15       |      215 |\n"
			"| 35 | Pro_35       |      235 |\n"
			"| 36 | Pro_36       |      236 |\n"
			"| 37 | Pro_37       |      237 |\n"
			"| 45 | Pro_45       |      245 |\n"
			"| 55 | Pro_55       |      255 |\n"
			"| 95 | Pro_95       |      295 |\n"
			"%s\n",
			linija_mat, linija_mat, linija_mat);
		printf("\n[Pritisnite ENTER za povratak u meni]");
		util_clear_line();
		break;

	case 4:
		system("cls");
		printf("4. NEPOSTOJECI PROIZVOD - OPIS\n");
		printf("=================================================================\n");
		printf("Pokusaj izdavanja proizvoda koji se ne nalazi u trenutnoj ponudi.\n\n");
		printf("Scenario:\n");
		printf("Nepostojeci proizvod + IZLAZ = GRESKA\n");
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 4: STARA MATICNA DATOTEKA (maticna.dat)\n");
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n"
			"| 20 | Pro_20       |      100 |\n"
			"| 30 | Pro_30       |      150 |\n"
			"| 40 | Pro_40       |      200 |\n"
			"| 50 | Pro_50       |      250 |\n"
			"| 60 | Pro_60       |      300 |\n"
			"| 70 | Pro_70       |      350 |\n"
			"| 80 | Pro_80       |      400 |\n"
			"| 90 | Pro_90       |      450 |\n"
			"%s\n",
			linija_mat, linija_mat, linija_mat);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 4: TRANSAKCIONA DATOTEKA (transakciona.dat - hronoloski)\n");
		printf("%s\n| Id | Promena  | Kolicina |\n%s\n"
			"| 30 | ULAZ     |      100 |\n"
			"| 92 | IZLAZ    |       40 |\n"
			"| 30 | IZLAZ    |       20 |\n"
			"| 23 | IZLAZ    |       40 |\n"
			"| 50 | ULAZ     |       20 |\n"
			"| 12 | IZLAZ    |       40 |\n"
			"| 22 | IZLAZ    |       20 |\n"
			"| 22 | IZLAZ    |       20 |\n"
			"%s\n",
			linija_tra, linija_tra, linija_tra);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 4: SUMARNA I SORTIRANA DATOTEKA PROMENA (tran_%s.dat)\n", datum);
		printf("%s\n| Id | Promena  | Kolicina |\n%s\n"
			"| 12 | IZLAZ    |       40 |\n"
			"| 22 | IZLAZ    |       40 |\n"
			"| 23 | IZLAZ    |       40 |\n"
			"| 30 | ULAZ     |       80 |\n"
			"| 50 | ULAZ     |       20 |\n"
			"| 92 | IZLAZ    |       40 |\n"
			"%s\n",
			linija_tra, linija_tra, linija_tra);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 4: NOVA MATICNA DATOTEKA (maticna.dat)\n");
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n"
			"| 20 | Pro_20       |      100 |\n"
			"| 30 | Pro_30       |      230 |\n"
			"| 40 | Pro_40       |      200 |\n"
			"| 50 | Pro_50       |      270 |\n"
			"| 60 | Pro_60       |      300 |\n"
			"| 70 | Pro_70       |      350 |\n"
			"| 80 | Pro_80       |      400 |\n"
			"| 90 | Pro_90       |      450 |\n"
			"%s\n",
			linija_mat, linija_mat, linija_mat);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 4: IZVESTAJ O PROMENAMA (prom_%s.rpt)\n", datum);
		printf("%s\n", linija_rpt);
		printf("|          Proizvod            |     Promena    |   Nova   |\n");
		printf("%s\n", linija_rpt);
		printf("| Id | Kolicina |    Naziv     | Tip | Kolicina | kolicina |\n");
		printf("%s\n", linija_rpt);
		printf("| 30 |      150 | Pro_30       |  +  |       80 |      230 |\n"
			"| 50 |      250 | Pro_50       |  +  |       20 |      270 |\n"
			"%s\n",
			linija_rpt);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 4: IZVESTAJ O GRESKAMA (err_pro_%s.rpt)\n", datum);
		printf("%s\n", linija_err);
		printf("|          Proizvod            |     Promena    |             Greska             |\n");
		printf("%s\n", linija_err);
		printf("| Id | Kolicina |    Naziv     | Tip | Kolicina |                                |\n");
		printf("%s\n", linija_err);
		printf("| 12 | X        | X            |  -  |       40 | Nepostojeci proizvod.          |\n"
			"| 22 | X        | X            |  -  |       40 | Nepostojeci proizvod.          |\n"
			"| 23 | X        | X            |  -  |       40 | Nepostojeci proizvod.          |\n"
			"| 92 | X        | X            |  -  |       40 | Nepostojeci proizvod.          |\n"
			"%s\n",
			linija_err);
		printf("\n[Pritisnite ENTER za povratak u meni]");
		util_clear_line();
		break;

	case 5:
		system("cls");
		printf("5. SVEOBUHVATNI SLUCAJ - OPIS\n");
		printf("=================================================================\n");
		printf("Objedinjuje sve moguce slucajeve azurianja baze proizvoda.\n");
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 5: STARA MATICNA DATOTEKA (maticna.dat)\n");
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n"
			"| 20 | Pro_20       |      100 |\n"
			"| 30 | Pro_30       |      150 |\n"
			"| 40 | Pro_40       |      200 |\n"
			"| 50 | Pro_50       |      250 |\n"
			"| 60 | Pro_60       |      300 |\n"
			"| 70 | Pro_70       |      350 |\n"
			"| 80 | Pro_80       |      400 |\n"
			"| 90 | Pro_90       |      450 |\n"
			"%s\n",
			linija_mat, linija_mat, linija_mat);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 5: TRANSAKCIONA DATOTEKA (transakciona.dat - hronoloski)\n");
		printf("%s\n| Id | Promena  | Kolicina |\n%s\n"
			"| 20 | ULAZ     |       50 |\n"
			"| 70 | IZLAZ    |       50 |\n"
			"| 90 | IZLAZ    |       50 |\n"
			"| 50 | IZLAZ    |      800 |\n"
			"| 60 | IZLAZ    |      800 |\n"
			"| 35 | ULAZ     |      150 |\n"
			"| 15 | ULAZ     |      150 |\n"
			"| 22 | IZLAZ    |      175 |\n"
			"| 92 | IZLAZ    |      175 |\n"
			"| 20 | ULAZ     |       50 |\n"
			"| 70 | IZLAZ    |       50 |\n"
			"| 90 | ULAZ     |       50 |\n"
			"%s\n",
			linija_tra, linija_tra, linija_tra);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 5: SUMARNA I SORTIRANA DATOTEKA PROMENA (tran_%s.dat)\n", datum);
		printf("%s\n| Id | Promena  | Kolicina |\n%s\n"
			"| 15 | ULAZ     |      150 |\n"
			"| 20 | ULAZ     |      100 |\n"
			"| 22 | IZLAZ    |      175 |\n"
			"| 35 | ULAZ     |      150 |\n"
			"| 50 | IZLAZ    |      800 |\n"
			"| 60 | IZLAZ    |      800 |\n"
			"| 70 | IZLAZ    |      100 |\n"
			"| 90 | ULAZ     |        0 |\n"
			"| 92 | IZLAZ    |      175 |\n"
			"%s\n",
			linija_tra, linija_tra, linija_tra);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 5: NOVA MATICNA DATOTEKA (maticna.dat)\n");
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n"
			"| 15 | Pro_15       |      150 |\n"
			"| 20 | Pro_20       |      200 |\n"
			"| 30 | Pro_30       |      150 |\n"
			"| 35 | Pro_35       |      150 |\n"
			"| 40 | Pro_40       |      200 |\n"
			"| 50 | Pro_50       |      250 |\n"
			"| 60 | Pro_60       |      300 |\n"
			"| 70 | Pro_70       |      250 |\n"
			"| 80 | Pro_80       |      400 |\n"
			"| 90 | Pro_90       |      450 |\n"
			"%s\n",
			linija_mat, linija_mat, linija_mat);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 5: IZVESTAJ O PROMENAMA (prom_%s.rpt)\n", datum);
		printf("%s\n", linija_rpt);
		printf("|          Proizvod            |     Promena    |   Nova   |\n");
		printf("%s\n", linija_rpt);
		printf("| Id | Kolicina |    Naziv     | Tip | Kolicina | kolicina |\n");
		printf("%s\n", linija_rpt);
		printf("| 20 |      100 | Pro_20       |  +  |      100 |      200 |\n"
			"| 70 |      350 | Pro_70       |  -  |      100 |      250 |\n"
			"| 90 |      450 | Pro_90       |  +  |        0 |      450 |\n"
			"%s\n",
			linija_rpt);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 5: IZVESTAJ O NOVIM PROIZVODIMA (nov_pro_%s.rpt)\n", datum);
		printf("%s\n| Id | Naziv        | Kolicina |\n%s\n"
			"| 15 | Pro_15       |      150 |\n"
			"| 35 | Pro_35       |      150 |\n"
			"%s\n",
			linija_mat, linija_mat, linija_mat);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 5: GRESKA NEPOSTOJECI PROIZVOD (err_pro_%s.rpt)\n", datum);
		printf("%s\n", linija_err);
		printf("|          Proizvod            |     Promena    |             Greska             |\n");
		printf("%s\n", linija_err);
		printf("| Id | Kolicina |    Naziv     | Tip | Kolicina |                                |\n");
		printf("%s\n", linija_err);
		printf("| 22 | X        | X            |  -  |      175 | Nepostojeci proizvod.          |\n"
			"| 92 | X        | X            |  -  |      175 | Nepostojeci proizvod.          |\n"
			"%s\n",
			linija_err);
		printf("\n[Pritisnite ENTER za narednu stranu]");
		util_clear_line();

		system("cls");
		printf("DEMO 5: GRESKA NEPOSTOJECA KOLICINA (err_kol_%s.rpt)\n", datum);
		printf("%s\n", linija_err);
		printf("|          Proizvod            |     Promena    |             Greska             |\n");
		printf("%s\n", linija_err);
		printf("| Id | Kolicina |    Naziv     | Tip | Kolicina |                                |\n");
		printf("%s\n", linija_err);
		printf("| 50 |      250 | Pro_50       |  -  |      800 | Nepostojeca kolicina proizvoda.|\n"
			"| 60 |      300 | Pro_60       |  -  |      800 | Nepostojeca kolicina proizvoda.|\n"
			"%s\n",
			linija_err);
		printf("\n[Pritisnite ENTER za povratak u meni]");
		util_clear_line();
		break;
	}
}

static void demo_1(void) { prikaziDemoTekst(1); }
static void demo_2(void) { prikaziDemoTekst(2); }
static void demo_3(void) { prikaziDemoTekst(3); }
static void demo_4(void) { prikaziDemoTekst(4); }
static void demo_5(void) { prikaziDemoTekst(5); }

static void run_demo_menu_wrapped(void) {
	Menu m;
	menu_init(&m, "MENI Demo");

	menu_add_item(&m, "Osnovni slucaj", demo_1);
	menu_add_item(&m, "Nepostojeca kolicina", demo_2);
	menu_add_item(&m, "Nov proizvod", demo_3);
	menu_add_item(&m, "Nepostojeci proizvod", demo_4);
	menu_add_item(&m, "Sveobuhvatni slucaj", demo_5);

	menu_run(&m);
	menu_free(&m);
}



static void prikaziPomocOAzu(void) {
	system("cls");
	printf("============================================================\n");
	printf(" O AZURIRANJU SERIJSKE DATOTEKE (ASD) - STRANA 1/3\n");
	printf("============================================================\n\n");
	printf("Azuriranje serijske datoteke predstavlja proces uskladjivanja\n");
	printf("stanja proizvoda u maticnoj datoteci sa promenama koje su\n");
	printf("zabelezene u transakcionoj datoteci.\n\n");
	printf("U klasicnoj obradi podataka, ovo je kljucni proces koji se\n");
	printf("obicno obavlja periodicno (npr. na kraju dana).\n");
	printf("U ovom sistemu koristimo dve kljucne datoteke:\n");
	printf("1. MATICNA DATOTEKA: Sadrzi trenutno stanje (Id, Naziv, Kolicina).\n");
	printf("   Ona je SERIJSKA (sortirana po Id-u u rastucem poretku).\n");
	printf("2. TRANSAKCIONA DATOTEKA: Sadrzi hronoloske promene (ULAZ/IZLAZ).\n");
	printf("   Ona je REDNA (nesortirana, zapisi idu redom kako nastaju).\n\n");
	printf("[Pritisnite ENTER za sledecu stranu...]");
	getchar();

	system("cls");
	printf("============================================================\n");
	printf(" O AZURIRANJU SERIJSKE DATOTEKE (ASD) - STRANA 2/3\n");
	printf("============================================================\n\n");
	printf("PROCES OBRADE:\n");
	printf("1. SUMIRANJE: Vise promena za isti Id se sabira u jedan slog.\n");
	printf("2. SORTIRANJE: Sumarne promene se sortiraju po Id-u.\n");
	printf("3. AZURIRANJE: Uporedjuju se stara maticna i sumarna datoteka.\n");
	printf("Rezultat je datoteka dnevnih promena (tran_ggmmdd.dat).\n");
	printf("Nakon toga, program sinhronizovano cita maticnu datoteku i\n");
	printf("datoteku promena i kreira novu maticnu datoteku.\n\n");
	printf("[Pritisnite ENTER za sledecu stranu...]");
	getchar();

	system("cls");
	printf("============================================================\n");
	printf(" O AZURIRANJU SERIJSKE DATOTEKE (ASD) - STRANA 3/3\n");
	printf("============================================================\n\n");
	printf("Tokom azuriranja sistem prati cetiri kljucna dogadjaja:\n");
	printf("1. Proizvod postoji u maticnoj datoteci.\n");
	printf("2. Proizvod NE postoji u maticnoj datoteci.\n");
	printf("3. Pokusaj povecanja kolicine (ULAZ).\n");
	printf("4. Pokusaj smanjenja kolicine (IZLAZ).\n");
	printf("Ishodi:\n");
	printf("1. Uspesno azuriranje (proizvod azuriran/dodat).\n");
	printf("2. Neuspesno azuriranje (greska - upisuje se u .rpt fajl).\n");
	printf("[Pritisnite ENTER za povratak u meni...]");
	getchar();
}

static void prikaziONama(void) {
	system("cls");
	printf("============================================================\n");
	printf(" O NAMA\n");
	printf("============================================================\n\n");
	printf("Softverski inzenjeri:\n\n");
	printf("1. Tijana Milosavljevic\n");
	printf("   e-mail: tm20220009@student.fon.bg.ac.rs\n\n");
	printf("2. Strahinja Mitov\n");
	printf("   e-mail: sm20220022@student.fon.bg.ac.rs\n\n");
	printf("Studenti 4. godine Fakulteta organizacionih nauka\n");
	printf("Smer: Informacioni sistemi i tehnologije\n");
	printf("Modul: Softversko inzenjerstvo\n\n");
	printf("[Pritisnite ENTER za povratak u meni...]");
	getchar();
}

static void run_help_menu(void) {
	Menu m;
	menu_init(&m, "MENI Pomoc");

	menu_add_item(&m, "O azuriranju serijske datoteke", prikaziPomocOAzu);
	menu_add_item(&m, "Demo", run_demo_menu_wrapped);
	menu_add_item(&m, "O nama", prikaziONama);

	menu_run(&m);
	menu_free(&m);
}

void ispisiZaglavlje(void) {
	printf("************************************************************************\n");
	printf("* Aplikacija: ASD\n");
	printf("* Opis      : Azuriranje serijske (sortirane redne) datoteke\n");
	printf("* Verzija   : 1.0\n");
	printf("* Upotreba  : asd.exe [-d[1|2|3|4|5]]\n");
	printf("                1 - demo za prvi slucaj azuriranja,\n");
	printf("                2 - demo za drugi slucaj azuriranja,\n");
	printf("                3 - demo za treci slucaj azuriranja,\n");
	printf("                4 - demo za cetvrti slucaj azuriranja,\n");
	printf("                5 - demo za peti slucaj azuriranja\n");

	printf("* Datum     : 10.02.2026.\n");
	printf("* Autori    : Tijana Milosavljevic, tm20220009@student.fon.bg.ac.rs,\n");
	printf("*             Strahinja Mitov, sm20220022@student.fon.bg.ac.rs\n");
	printf("* Mentor    : Sasa D. Lazarevic,  slazar@fon.rs\n");
	printf("************************************************************************\n\n");
}

static void akcija_nije_postavljena(void) {
	printf("\nINFO: Funkcija azuriranja nije postavljena za ovaj projekat.\n");
	printf("Pritisnite ENTER za povratak...");
	int c; while ((c = getchar()) != '\n' && c != EOF);
}

void adts_run_main_menu(void) {
	if (!g_updateAllAction) {
		g_updateAllAction = akcija_nije_postavljena;
	}

	ispisiZaglavlje();

	Menu m;
	menu_init(&m, "GLAVNI MENI");

	menu_add_item(&m, "Rad sa transakcionom datotekom", run_transakcija_menu);
	menu_add_item(&m, "Rad sa maticnom datotekom", run_maticna_menu);
	menu_add_item(&m, "Pomoc", run_help_menu);

	menu_run(&m);
	menu_free(&m);
}

void adts_set_update_all_action(MenuAction action) {
	g_updateAllAction = action;
}