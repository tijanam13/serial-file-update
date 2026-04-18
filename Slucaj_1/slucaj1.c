#define _CRT_SECURE_NO_WARNINGS
#include "../Biblioteka/adts.h"
#include "../Biblioteka/misc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void procesuirajSlucaj1(void) {
	char datum[7];
	char oldMatPutanja[100], sumTranPutanja[100], rptPutanja[100];
	char demoMatPutanja[100], demoTranPutanja[100];
	char rptImeFajla[64];

	formirajDatum(datum);
	sprintf(rptImeFajla, "prom_%s.rpt", datum);

	sprintf(oldMatPutanja, ".\\DATA\\OLD\\mat_%s.dat", datum);
	sprintf(sumTranPutanja, ".\\DATA\\OLD\\tran_%s.dat", datum);
	sprintf(rptPutanja, ".\\RPT\\%s", rptImeFajla);
	sprintf(demoMatPutanja, ".\\DEMO\\maticna.dat");
	sprintf(demoTranPutanja, ".\\DEMO\\SLUC_1\\transakciona.dat");

	obaveznoKopirajAkoNePostoji(MAT_DAT, demoMatPutanja);
	obaveznoKopirajAkoNePostoji(TRAN_DAT, demoTranPutanja);
	kopirajFajl(MAT_DAT, oldMatPutanja);

	VektorTransakcija vt;
	vt_init(&vt, 10);
	if (!dat_ucitaj_transakcije(TRAN_DAT, &vt)) {
		printf("Greska: Transakciona datoteka je prazna!\n");
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

	FILE* fMat = fopen(MAT_DAT, "rb");
	FILE* fNovaMat = fopen(".\\DATA\\maticna_nova.dat", "wb");
	FILE* fRpt = fopen(rptPutanja, "w");

	if (!fMat || !fNovaMat || !fRpt) {
		printf("Greska: Problemi sa otvaranjem datoteka!\n");
		vt_free(&vt); vt_free(&vtSum);
		return;
	}

	fprintf(fRpt, "Izvestaj o promenama za dan %s: %s\n", datum, rptImeFajla);

	fprintf(fRpt, "+----+----------+--------------+-----+----------+----------+\n");
	fprintf(fRpt, "|          Proizvod            |    Promena     |   Nova   |\n");
	fprintf(fRpt, "+----+----------+--------------+-----+----------+          +\n");
	fprintf(fRpt, "| Id | Kolicina |    Naziv     | Tip | Kolicina | kolicina |\n");
	fprintf(fRpt, "+----+----------+--------------+-----+----------+----------+\n");

	PROIZVOD mp;
	size_t s_idx = 0;

	while (fread(&mp, sizeof(PROIZVOD), 1, fMat)) {
		if (s_idx < vtSum.size && mp.Id == vtSum.items[s_idx].Id) {
			unsigned staraKol = mp.Kolicina;
			TRANSAKCIJA st = vtSum.items[s_idx];
			int promVal = (st.Promena == ULAZ) ? (int)st.Kolicina : -(int)st.Kolicina;
			mp.Kolicina += (unsigned)promVal;


			fprintf(fRpt, "| %-2u | %8u | %-12.12s |  %c  | %8u | %8u |\n",
				mp.Id,
				staraKol,
				mp.Naziv,
				(st.Promena == ULAZ ? '+' : '-'),
				st.Kolicina,
				mp.Kolicina);

			s_idx++;
		}
		fwrite(&mp, sizeof(PROIZVOD), 1, fNovaMat);
	}

	fprintf(fRpt, "+----+----------+--------------+-----+----------+----------+\n");

	fclose(fMat); fclose(fNovaMat); fclose(fRpt);

	remove(MAT_DAT);
	rename(".\\DATA\\maticna_nova.dat", MAT_DAT);

	system("cls");
	printf("INFO: Azuriranje maticne datoteke je uspesno izvrseno.\n");

	prikaziSadrzajIzvestaja(rptPutanja);

	vt_free(&vt);
	vt_free(&vtSum);

	printf("\nINFO: Slucaj 1 (Osnovni slucaj) uspesno obradjen.\n");
}