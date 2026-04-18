#define _CRT_SECURE_NO_WARNINGS
#include "misc.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void formirajDatum(char* out) {
	time_t now = time(NULL);
	struct tm* t = localtime(&now);
	if (t) sprintf(out, "%02d%02d%02d", t->tm_year % 100, t->tm_mon + 1, t->tm_mday);
}

int kopirajFajl(const char* src, const char* dest) {
	FILE* f1 = fopen(src, "rb");
	if (!f1) return 0;
	FILE* f2 = fopen(dest, "wb");
	if (!f2) { fclose(f1); return 0; }

	char buffer[4096];
	size_t n;
	while ((n = fread(buffer, 1, sizeof(buffer), f1)) > 0) fwrite(buffer, 1, n, f2);

	fclose(f1);
	fclose(f2);
	return 1;
}

int obaveznoKopirajAkoNePostoji(const char* cilj, const char* izvor) {
	FILE* f = fopen(cilj, "rb");
	if (f) { fclose(f); return 1; }
	int ok = kopirajFajl(izvor, cilj);
	if (ok) printf("INFO: Fajl %s je kopiran iz DEMO foldera.\n", cilj);
	else printf("Greska: Ne mogu da kopiram %s iz %s\n", cilj, izvor);
	return ok;
}

int uporediTransakcije(const void* a, const void* b) {
	return (((SUM_TRAN*)a)->Id - ((SUM_TRAN*)b)->Id);
}


void prikaziSadrzajIzvestaja(const char* putanja) {
	FILE* f = fopen(putanja, "r");
	if (!f) return;

	printf("\n--- SADRZAJ IZVESTAJA: %s ---\n", putanja);
	char linija[256];
	int brojac = 0;
	while (fgets(linija, sizeof(linija), f)) {
		printf("%s", linija);
		if (++brojac % 20 == 0) {
			printf("\n-- Pritisnite ENTER za nastavak --");
			util_clear_line();
		}
	}
	fclose(f);
	printf("\n--- KRAJ IZVESTAJA ---\n");
}


void kreirajMaticnuDatoteku(void) {
	FILE* f = fopen(MAT_DAT, "rb");
	if (f) {
		fclose(f);
		char odgovor;
		printf("U folderu sa podacima vec postoji maticna datoteka.\n");
		printf("Da li zelite da je obrisete i kreirate novu maticnu datoteku? (D/N) ");

		(void)scanf(" %c", &odgovor);
		util_clear_line();

		if (odgovor != 'D' && odgovor != 'd') return;
	}

	f = fopen(MAT_DAT, "wb");
	if (f) {
		fclose(f);
		printf("INFO: Maticna datoteka je kreirana.\n");
	}
	else {
		printf("ERROR: Maticna datoteka nije kreirana.\n");
	}
}

void obrisiMaticnuDatoteku(void) {
	if (remove(MAT_DAT) == 0) {
		printf("INFO: Maticna datoteka je obrisana.\n");
	}
	else {
		printf("ERROR: Maticna datoteka nije obrisana.\n");
	}
}
void ubaciProizvod(void) {
	PROIZVOD novi;

	printf("\n--- DODAVANJE NOVOG PROIZVODA ---\n");
	if (!util_read_uint("Unesite Id proizvoda: ", &novi.Id)) return;
	if (!util_read_token("Unesite naziv: ", novi.Naziv, 64)) return;
	if (!util_read_uint("Unesite kolicinu: ", &novi.Kolicina)) return;

	VektorProizvod vp;
	vp_init(&vp, 10);

	dat_ucitaj_maticnu(MAT_DAT, &vp);

	if (vp_find_by_id(&vp, novi.Id) != -1) {
		printf("ERROR: Proizvod sa Id = %u vec postoji u maticnoj datoteci.\n", novi.Id);
	}
	else {
		size_t i = 0;
		while (i < vp.size && vp.items[i].Id < novi.Id) {
			i++;
		}

		vp_insert_at(&vp, i, &novi);

		if (dat_sacuvaj_maticnu(MAT_DAT, &vp)) {
			printf("INFO: Proizvod je uspesno dodat. Maticna datoteka je sortirana.\n");
		}
		else {
			printf("ERROR: Neuspesno snimanje u maticnu datoteku.\n");
		}
	}

	vp_free(&vp);
}

void obrisiProizvod(void) {
	unsigned id;
	if (!util_read_uint("Unesite identifikator proizvoda za brisanje: ", &id)) return;

	VektorProizvod vp;
	vp_init(&vp, 10);
	dat_ucitaj_maticnu(MAT_DAT, &vp);

	int idx = vp_find_by_id(&vp, id);

	if (idx == -1) {
		printf("Proizvod sa zadatim identifikatorom ne postoji.\n");
	}
	else {
		printf("Pronadjen proizvod: ID: %u, Naziv: %s, Kolicina: %u\n",
			vp.items[idx].Id, vp.items[idx].Naziv, vp.items[idx].Kolicina);

		printf("Da li ste sigurni da zelite da obrisete navedeni proizvod? (D/N) ");

		char odg;
		(void)scanf(" %c", &odg);
		util_clear_line();

		if (odg == 'D' || odg == 'd') {
			vp_remove_by_index(&vp, (size_t)idx);
			dat_sacuvaj_maticnu(MAT_DAT, &vp);
			printf("INFO: Proizvod je uspesno obrisan.\n");
		}
	}
	vp_free(&vp);
}
void azurirajJedanProizvod(void) {
	unsigned id;
	if (!util_read_uint("Unesite identifikator proizvoda: ", &id)) return;

	FILE* f = fopen(MAT_DAT, "r+b");
	if (!f) {
		printf("INFO: Maticna datoteka ne postoji.\n");
		return;
	}

	PROIZVOD p;
	int pronadjeno = 0;

	while (fread(&p, sizeof(PROIZVOD), 1, f)) {
		if (p.Id == id) {
			pronadjeno = 1;

			printf("\nPronadjen proizvod:\n");
			printf("ID: %u | Naziv: %s | Trenutna kolicina: %u\n", p.Id, p.Naziv, p.Kolicina);
			printf("--------------------------------------\n");

			if (!util_read_token("Unesite novi naziv: ", p.Naziv, 64)) {
				fclose(f);
				return;
			}
			if (!util_read_uint("Unesite novu kolicinu: ", &p.Kolicina)) {
				fclose(f);
				return;
			}

			fseek(f, -(long)sizeof(PROIZVOD), SEEK_CUR);

			if (fwrite(&p, sizeof(PROIZVOD), 1, f) == 1) {
				printf("INFO: Proizvod je uspesno azuriran.\n");
			}
			else {
				printf("ERROR: GRESKA prilikom upisa u datoteku.\n");
			}

			break;
		}
	}

	if (!pronadjeno) {
		printf("INFO: Proizvod sa zadatim identifikatorom ne postoji.\n");
	}

	fclose(f);
}

void prikaziSveProizvode(void) {
	VektorProizvod vp;
	vp_init(&vp, 10);

	if (!dat_ucitaj_maticnu(MAT_DAT, &vp) || vp.size == 0) {
		printf("INFO: Maticna datoteka je prazna ili ne postoji.\n");
		vp_free(&vp);
		return;
	}

	int stavkiPoStrani = 10;

	for (size_t i = 0; i < vp.size; i++) {
		if (i % stavkiPoStrani == 0) {
			printf("\n%-7s %-15s %-10s\n", "Id", "Naziv", "Kolicina");
			printf("--------------------------------------\n");
		}

		printf("%-7u %-15s %-10u\n", vp.items[i].Id, vp.items[i].Naziv, vp.items[i].Kolicina);

		if ((i + 1) % stavkiPoStrani == 0 && (i + 1) < vp.size) {
			printf("\n-- Strana %zu. Pritisnite ENTER za sledecu stranu --", (i / stavkiPoStrani) + 1);
			util_clear_line();
		}
	}

	printf("--------------------------------------\n");
	printf("INFO: Kraj prikaza (ukupno proizvoda: %zu).\n", vp.size);

	vp_free(&vp);
}

void prikaziJedanProizvod(void) {
	unsigned id;
	if (!util_read_uint("Unesite identifikator proizvoda: ", &id)) return;

	FILE* f = fopen(MAT_DAT, "rb");
	if (!f) {
		printf("INFO: Maticna datoteka ne postoji.\n");
		return;
	}

	PROIZVOD p;
	int pronadjeno = 0;

	while (fread(&p, sizeof(PROIZVOD), 1, f)) {
		if (p.Id == id) {
			printf("\nPodaci o proizvodu u maticnoj datoteci:\n");
			printf("--------------------------------------\n");
			printf("%-10s %-15s %-10s\n", "Id", "Naziv", "Kolicina");
			printf("--------------------------------------\n");
			printf("%-10u %-15s %-10u\n", p.Id, p.Naziv, p.Kolicina);
			printf("--------------------------------------\n");
			pronadjeno = 1;
			break;
		}
	}

	fclose(f);

	if (!pronadjeno) {
		printf("INFO: Proizvod sa zadatim identifikatorom ne postoji u maticnoj datoteci.\n");
	}
}


void kreirajTransakcionuDatoteku(void) {
	FILE* f = fopen(TRAN_DAT, "rb");
	if (f) {
		fclose(f);
		char odgovor;
		printf("U folderu sa podacima vec postoji transakciona datoteka.\n");
		printf("Da li zelite da je obrisete i kreirate novu transakcionu datoteku? (D/N): ");
		(void)scanf(" %c", &odgovor);
		util_clear_line();

		if (odgovor != 'D' && odgovor != 'd') {
			return;
		}
	}

	f = fopen(TRAN_DAT, "wb");
	if (f) {
		fclose(f);
		printf("INFO: Transakciona datoteka je kreirana.\n");
	}
	else {
		printf("ERROR: Transakciona datoteka nije kreirana.\n");
	}
}

void obrisiTransakcionuDatoteku(void) {
	if (remove(TRAN_DAT) == 0) {
		printf("INFO: Transakciona datoteka je obrisana.\n");
	}
	else {

		printf("ERROR: Transakciona datoteka nije obrisana.\n");
	}
}

void ubaciTransakciju(void) {
	TRANSAKCIJA t;
	int tip;

	printf("\n--- UNOS NOVE TRANSAKCIJE ---\n");
	printf("(Napomena: Za opoziv transakcije unesite isti Id i Kolicinu, ali suprotan Tip)\n");

	if (!util_read_uint("Unesite Id proizvoda: ", &t.Id)) return;

	printf("Unesite tip promene (1 za ULAZ, -1 za IZLAZ): ");
	if (scanf("%d", &tip) != 1) {
		util_clear_line();
		printf("ERROR: Nevazeci unos za tip.\n");
		return;
	}
	util_clear_line();

	if (tip != 1 && tip != -1) {
		printf("ERROR: Tip mora biti 1 (ULAZ) ili -1 (IZLAZ).\n");
		return;
	}
	t.Promena = (tip == 1) ? ULAZ : IZLAZ;

	if (!util_read_uint("Unesite kolicinu: ", &t.Kolicina)) return;

	FILE* f = fopen(TRAN_DAT, "ab");
	if (f) {
		if (fwrite(&t, sizeof(TRANSAKCIJA), 1, f) == 1) {
			printf("INFO: Transakcija je uspesno sacuvana.\n");
		}
		else {
			printf("ERROR: Neuspesan upis u transakcionu datoteku.\n");
		}
		fclose(f);
	}
	else {
		printf("ERROR: Ne mogu da otvorim transakcionu datoteku za pisanje.\n");
	}
}

void prikaziSveTransakcije(void) {
	FILE* f = fopen(TRAN_DAT, "rb");

	if (!f) {
		printf("INFO: Transakciona datoteka ne postoji.\n");
		return;
	}

	TRANSAKCIJA t;
	int brojac = 0;
	int pronadjeno = 0;

	printf("\n%-10s %-10s %-10s\n", "Id", "Promena", "Kolicina");
	printf("----------------------------------\n");

	while (fread(&t, sizeof(TRANSAKCIJA), 1, f)) {
		pronadjeno = 1;
		printf("%-10u %-10s %-10u\n",
			t.Id,
			(t.Promena == ULAZ) ? "ULAZ" : "IZLAZ",
			t.Kolicina);

		brojac++;

		if (brojac % 15 == 0) {
			printf("\n-- Pritisnite ENTER za nastavak prikaza --");
			util_clear_line();
			printf("\n%-10s %-10s %-10s\n", "Id", "Promena", "Kolicina");
			printf("----------------------------------\n");
		}
	}

	if (!pronadjeno) {
		printf("INFO: Transakciona datoteka je prazna.\n");
	}

	fclose(f);
	printf("----------------------------------\n");
}

void prikaziTransakcijeJednogProizvoda(void) {
	unsigned id;
	if (!util_read_uint("Unesite identifikator proizvoda: ", &id)) return;

	FILE* f = fopen(TRAN_DAT, "rb");
	if (!f) {
		printf("INFO: Transakciona datoteka ne postoji.\n");
		return;
	}

	TRANSAKCIJA t;
	int pronadjeno = 0;

	while (fread(&t, sizeof(TRANSAKCIJA), 1, f)) {
		if (t.Id == id) {
			if (!pronadjeno) {
				printf("\nTransakcije za proizvod sa ID %u:\n", id);
				printf("%-12s %-10s\n", "Tip promene", "Kolicina");
				printf("--------------------------\n");
			}
			printf("%-12s %-10u\n", (t.Promena == ULAZ ? "ULAZ" : "IZLAZ"), t.Kolicina);
			pronadjeno = 1;
		}
	}

	fclose(f);

	if (!pronadjeno) {
		printf("INFO: Ne postoje transakcije navedenog proizvoda.\n");
	}
	else {
		printf("--------------------------\n");
	}
}





