#define _CRT_SECURE_NO_WARNINGS
#include "../Biblioteka/adts.h"
#include "../Biblioteka/misc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void procesuirajSlucaj5(void) {
	char datum[7];
	char oldMatPutanja[100], sumTranPutanja[100];
	char rptProm[100], rptNovPro[100], rptErrKol[100], rptErrPro[100];
	char demoMatPutanja[100], demoTranPutanja[100];

	formirajDatum(datum);

	sprintf(oldMatPutanja, ".\\DATA\\OLD\\mat_%s.dat", datum);
	sprintf(sumTranPutanja, ".\\DATA\\OLD\\tran_%s.dat", datum);

	sprintf(rptProm, ".\\RPT\\prom_%s.rpt", datum);
	sprintf(rptNovPro, ".\\RPT\\nov_pro_%s.rpt", datum);
	sprintf(rptErrKol, ".\\ERR\\err_kol_%s.rpt", datum);
	sprintf(rptErrPro, ".\\ERR\\err_pro_%s.rpt", datum);

	sprintf(demoMatPutanja, ".\\DEMO\\maticna.dat");
	sprintf(demoTranPutanja, ".\\DEMO\\SLUC_5\\transakciona.dat");

	obaveznoKopirajAkoNePostoji(MAT_DAT, demoMatPutanja);
	obaveznoKopirajAkoNePostoji(TRAN_DAT, demoTranPutanja);
	kopirajFajl(MAT_DAT, oldMatPutanja);

	VektorTransakcija vt;
	vt_init(&vt, 10);
	if (!dat_ucitaj_transakcije(TRAN_DAT, &vt)) {
		printf("Greska: Transakciona datoteka je prazna!\n");
		vt_free(&vt); return;
	}

	VektorTransakcija vtIdSet;
	vt_init(&vtIdSet, 10);
	for (size_t i = 0; i < vt.size; i++) {
		if (vt_find_by_id(&vtIdSet, vt.items[i].Id) == -1) {
			vt_push(&vtIdSet, &vt.items[i]);
		}
	}
	qsort(vtIdSet.items, vtIdSet.size, sizeof(TRANSAKCIJA), uporediTransakcije);

	for (size_t i = 1; i < vt.size; i++) {
		TRANSAKCIJA tmp = vt.items[i];
		int j = (int)i - 1;
		while (j >= 0 && vt.items[j].Id > tmp.Id) {
			vt.items[j + 1] = vt.items[j];
			j--;
		}
		vt.items[j + 1] = tmp;
	}

	FILE* fSumLog = fopen(sumTranPutanja, "wb");
	if (fSumLog) {
		fwrite(vtIdSet.items, sizeof(TRANSAKCIJA), vtIdSet.size, fSumLog);
		fclose(fSumLog);
	}

	FILE* fMat = fopen(MAT_DAT, "rb");
	FILE* fNovaMat = fopen(".\\DATA\\maticna_nova.dat", "wb");
	FILE* fProm = fopen(rptProm, "w");
	FILE* fNovi = fopen(rptNovPro, "w");
	FILE* fEKol = fopen(rptErrKol, "w");
	FILE* fEPro = fopen(rptErrPro, "w");

	if (!fMat || !fNovaMat || !fProm || !fNovi || !fEKol || !fEPro) {
		printf("Greska: Ne mogu da otvorim sve izvestaje za upis!\n");
		return;
	}

	fprintf(fProm, "Izvestaj o promenama (Slucaj 5) - %s\n", datum);
	fprintf(fProm, "+----+----------+--------------+-----+----------+----------+\n");
	fprintf(fProm, "|          Proizvod            |    Promena     |   Nova   |\n");
	fprintf(fProm, "+----+----------+--------------+-----+----------+          +\n");
	fprintf(fProm, "| Id | Kolicina |    Naziv     | Tip | Kolicina | kolicina |\n");
	fprintf(fProm, "+----+----------+--------------+-----+----------+----------+\n");

	fprintf(fNovi, "Izvestaj o novim proizvodima - %s\n", datum);
	fprintf(fNovi, "+----+----------------------+----------+\n");
	fprintf(fNovi, "| Id |        Naziv         | Kolicina |\n");
	fprintf(fNovi, "+----+----------------------+----------+\n");

	fprintf(fEKol, "Izvestaj o greskama (Kolicina) - %s\n", datum);
	fprintf(fEKol, "+----+----------+--------------+-----+----------+---------------------------------+\n");
	fprintf(fEKol, "|          Proizvod            |    Promena     |                                 |\n");
	fprintf(fEKol, "+----+----------+--------------+-----+----------+              Greska             +\n");
	fprintf(fEKol, "| Id | Kolicina |    Naziv     | Tip | Kolicina |                                 |\n");
	fprintf(fEKol, "+----+----------+--------------+-----+----------+---------------------------------+\n");

	fprintf(fEPro, "Izvestaj o greskama (Proizvod) - %s\n", datum);
	fprintf(fEPro, "+----+----------+--------------+-----+----------+--------------------------------+\n");
	fprintf(fEPro, "|          Proizvod            |    Promena     |                                |\n");
	fprintf(fEPro, "+----+----------+--------------+-----+----------+             Greska             +\n");
	fprintf(fEPro, "| Id | Kolicina |    Naziv     | Tip | Kolicina |                                |\n");
	fprintf(fEPro, "+----+----------+--------------+-----+----------+--------------------------------+\n");

	PROIZVOD mp;
	int imaMat = (fread(&mp, sizeof(PROIZVOD), 1, fMat) == 1);
	size_t id_idx = 0;
	size_t t_idx = 0;

	while (imaMat || id_idx < vtIdSet.size) {

		if (imaMat && id_idx < vtIdSet.size && mp.Id == vtIdSet.items[id_idx].Id) {

			unsigned pocetnaKol = mp.Kolicina;
			int netoPromena = 0;
			int biloValidnih = 0;

			while (t_idx < vt.size && vt.items[t_idx].Id == mp.Id) {
				TRANSAKCIJA st = vt.items[t_idx];

				if (st.Promena == IZLAZ && st.Kolicina > mp.Kolicina) {
					fprintf(fEKol, "| %-2u | %8u | %-12.12s |  -  | %8u | %-30s |\n",
						mp.Id, mp.Kolicina, mp.Naziv, st.Kolicina, "Nepostojeca kolicina proizvoda.");
				}
				else {
					int promVal = (st.Promena == ULAZ) ? (int)st.Kolicina : -(int)st.Kolicina;
					mp.Kolicina = (unsigned)((int)mp.Kolicina + promVal);
					netoPromena += promVal;
					biloValidnih = 1;
				}
				t_idx++;
			}

			if (biloValidnih) {
				char tip = (netoPromena >= 0) ? '+' : '-';
				unsigned absPromena = (netoPromena >= 0) ? (unsigned)netoPromena : (unsigned)(-netoPromena);

				fprintf(fProm, "| %-2u | %8u | %-12.12s |  %c  | %8u | %8u |\n",
					mp.Id, pocetnaKol, mp.Naziv, tip, absPromena, mp.Kolicina);
			}

			fwrite(&mp, sizeof(PROIZVOD), 1, fNovaMat);
			imaMat = (fread(&mp, sizeof(PROIZVOD), 1, fMat) == 1);
			id_idx++;
		}
		else if (imaMat && (id_idx >= vtIdSet.size || mp.Id < vtIdSet.items[id_idx].Id)) {
			fwrite(&mp, sizeof(PROIZVOD), 1, fNovaMat);
			imaMat = (fread(&mp, sizeof(PROIZVOD), 1, fMat) == 1);
		}
		else if (id_idx < vtIdSet.size && (!imaMat || vtIdSet.items[id_idx].Id < mp.Id)) {

			unsigned novaKol = 0;
			int imaUlaz = 0;

			while (t_idx < vt.size && vt.items[t_idx].Id == vtIdSet.items[id_idx].Id) {
				TRANSAKCIJA st = vt.items[t_idx];
				if (st.Promena == ULAZ) {
					novaKol += st.Kolicina;
					imaUlaz = 1;
				}
				else {
					fprintf(fEPro, "| %-2u | %8s | %12s |  -  | %8u | %-30s |\n",
						st.Id, "X", "X", st.Kolicina, "Nepostojeci proizvod.");
				}
				t_idx++;
			}

			if (imaUlaz) {
				PROIZVOD novi;
				novi.Id = vtIdSet.items[id_idx].Id;
				sprintf(novi.Naziv, "Pro_%u", novi.Id);
				novi.Kolicina = novaKol;
				fprintf(fNovi, "| %-2u | %-20s | %8u |\n", novi.Id, novi.Naziv, novi.Kolicina);
				fwrite(&novi, sizeof(PROIZVOD), 1, fNovaMat);
			}
			id_idx++;
		}
	}

	fprintf(fProm, "+----+----------+--------------+-----+----------+----------+\n");
	fprintf(fNovi, "+----+----------------------+----------+\n");
	fprintf(fEKol, "+----+----------+--------------+-----+----------+---------------------------------+\n");
	fprintf(fEPro, "+----+----------+--------------+-----+----------+--------------------------------+\n");

	fclose(fMat); fclose(fNovaMat); fclose(fProm); fclose(fNovi); fclose(fEKol); fclose(fEPro);
	vt_free(&vt);
	vt_free(&vtIdSet);

	remove(MAT_DAT);
	rename(".\\DATA\\maticna_nova.dat", MAT_DAT);


	system("cls");
	printf("INFO: Azuriranje maticne datoteke uspesno izvrseno.\n");

	prikaziSadrzajIzvestaja(rptProm);

	printf("\n[Pritisnite ENTER za sledeci izvestaj: Novi proizvodi]");
	getchar(); system("cls");

	prikaziSadrzajIzvestaja(rptNovPro);

	printf("\n[Pritisnite ENTER za sledeci izvestaj: Greske (Kolicina)]");
	getchar(); system("cls");

	prikaziSadrzajIzvestaja(rptErrKol);

	printf("\n[Pritisnite ENTER za sledeci izvestaj: Greske (Proizvod)]");
	getchar(); system("cls");

	prikaziSadrzajIzvestaja(rptErrPro);

	printf("\nINFO: Slucaj 5 (Sveobuhvatni slucaj) uspesno obradjen.\n");
}