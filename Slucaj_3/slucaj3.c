#define _CRT_SECURE_NO_WARNINGS
#include "../Biblioteka/adts.h"
#include "../Biblioteka/misc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void procesuirajSlucaj3(void) {
	char datum[7];
	char oldMatPutanja[100], sumTranPutanja[100], rptPutanja[100], novProPutanja[100];
	char demoMatPutanja[100], demoTranPutanja[100];
	char rptIme[64], novProIme[64];

	formirajDatum(datum);
	sprintf(rptIme, "prom_%s.rpt", datum);
	sprintf(novProIme, "nov_pro_%s.rpt", datum);

	sprintf(oldMatPutanja, ".\\DATA\\OLD\\mat_%s.dat", datum);
	sprintf(sumTranPutanja, ".\\DATA\\OLD\\tran_%s.dat", datum);
	sprintf(rptPutanja, ".\\RPT\\%s", rptIme);
	sprintf(novProPutanja, ".\\RPT\\%s", novProIme);

	sprintf(demoMatPutanja, ".\\DEMO\\maticna.dat");
	sprintf(demoTranPutanja, ".\\DEMO\\SLUC_3\\transakciona.dat");

	obaveznoKopirajAkoNePostoji(MAT_DAT, demoMatPutanja);
	obaveznoKopirajAkoNePostoji(TRAN_DAT, demoTranPutanja);
	kopirajFajl(MAT_DAT, oldMatPutanja);

	VektorTransakcija vt;
	vt_init(&vt, 10);
	if (!dat_ucitaj_transakcije(TRAN_DAT, &vt)) {
		printf("Greska: Transakciona datoteka za slucaj 3 je prazna!\n");
		vt_free(&vt); return;
	}

	VektorTransakcija vtSum;
	vt_init(&vtSum, 10);
	for (size_t i = 0; i < vt.size; i++) {
		int idx = vt_find_by_id(&vtSum, vt.items[i].Id);
		if (idx != -1) {
			int bilans = (vtSum.items[idx].Kolicina * vtSum.items[idx].Promena) +
				(vt.items[i].Kolicina * vt.items[i].Promena);
			vtSum.items[idx].Promena = (bilans >= 0) ? ULAZ : IZLAZ;
			vtSum.items[idx].Kolicina = abs(bilans);
		}
		else {
			vt_push(&vtSum, &vt.items[i]);
		}
	}
	qsort(vtSum.items, vtSum.size, sizeof(TRANSAKCIJA), uporediTransakcije);

	FILE* fSum = fopen(sumTranPutanja, "wb");
	if (fSum) {
		fwrite(vtSum.items, sizeof(TRANSAKCIJA), vtSum.size, fSum);
		fclose(fSum);
	}

	FILE* fMat = fopen(MAT_DAT, "rb");
	FILE* fNovaMat = fopen(".\\DATA\\maticna_nova.dat", "wb");
	FILE* fRpt = fopen(rptPutanja, "w");
	FILE* fNovPro = fopen(novProPutanja, "w");

	if (!fMat || !fNovaMat || !fRpt || !fNovPro) {
		printf("Greska: Problem sa otvaranjem fajlova!\n");
		return;
	}

	fprintf(fRpt, "Izvestaj o promenama za dan %s: %s\n", datum, rptIme);
	fprintf(fRpt, "+----+----------+--------------+-----+----------+----------+\n");
	fprintf(fRpt, "|          Proizvod            |    Promena     |   Nova   |\n");
	fprintf(fRpt, "+----+----------+--------------+-----+----------+          +\n");
	fprintf(fRpt, "| Id | Kolicina |    Naziv     | Tip | Kolicina | kolicina |\n");
	fprintf(fRpt, "+----+----------+--------------+-----+----------+----------+\n");

	fprintf(fNovPro, "Izvestaj o novim proizvodima za dan %s: %s\n", datum, novProIme);
	fprintf(fNovPro, "+----+----------------------+----------+\n");
	fprintf(fNovPro, "| Id |        Naziv         | Kolicina |\n");
	fprintf(fNovPro, "+----+----------------------+----------+\n");

	PROIZVOD mp;
	int imaMat = (fread(&mp, sizeof(PROIZVOD), 1, fMat) == 1);
	size_t s_idx = 0;

	while (imaMat || s_idx < vtSum.size) {
		if (imaMat && s_idx < vtSum.size && mp.Id == vtSum.items[s_idx].Id) {
			unsigned staraKol = mp.Kolicina;
			TRANSAKCIJA st = vtSum.items[s_idx];
			int promVal = (st.Promena == ULAZ) ? (int)st.Kolicina : -(int)st.Kolicina;
			mp.Kolicina += (unsigned)promVal;

			fprintf(fRpt, "| %-2u | %8u | %-12.12s |  %c  | %8u | %8u |\n",
				mp.Id, staraKol, mp.Naziv, (st.Promena == ULAZ ? '+' : '-'), st.Kolicina, mp.Kolicina);

			fwrite(&mp, sizeof(PROIZVOD), 1, fNovaMat);
			imaMat = (fread(&mp, sizeof(PROIZVOD), 1, fMat) == 1);
			s_idx++;
		}
		else if (imaMat && (s_idx >= vtSum.size || mp.Id < vtSum.items[s_idx].Id)) {
			fwrite(&mp, sizeof(PROIZVOD), 1, fNovaMat);
			imaMat = (fread(&mp, sizeof(PROIZVOD), 1, fMat) == 1);
		}
		else if (s_idx < vtSum.size && (!imaMat || vtSum.items[s_idx].Id < mp.Id)) {
			PROIZVOD novi;
			novi.Id = vtSum.items[s_idx].Id;
			sprintf(novi.Naziv, "Pro_%u", novi.Id);
			novi.Kolicina = vtSum.items[s_idx].Kolicina;

			fprintf(fNovPro, "| %-2u | %-20s | %8u |\n", novi.Id, novi.Naziv, novi.Kolicina);

			fwrite(&novi, sizeof(PROIZVOD), 1, fNovaMat);
			s_idx++;
		}
	}

	fprintf(fRpt, "+----+----------+--------------+-----+----------+----------+\n");
	fprintf(fNovPro, "+----+----------------------+----------+\n");

	fclose(fMat); fclose(fNovaMat); fclose(fRpt); fclose(fNovPro);
	vt_free(&vt); vt_free(&vtSum);

	remove(MAT_DAT);
	rename(".\\DATA\\maticna_nova.dat", MAT_DAT);


	system("cls");
	printf("INFO: Azuriranje maticne datoteke uspesno izvrseno.\n");
	prikaziSadrzajIzvestaja(rptPutanja);

	printf("\n[Pritisnite ENTER za sledeci izvestaj: Izvestaj o novim proizvodima]");
	getchar(); 

	system("cls");
	prikaziSadrzajIzvestaja(novProPutanja);

	printf("\nINFO: Slucaj 3 (Nov proizvod) uspesno obradjen.\n");
}