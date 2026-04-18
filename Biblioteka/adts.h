#ifndef ADTS_H
#define ADTS_H

#include <stdlib.h>
#include <stdio.h>
#include "defs.h"

#define DATA_FOLDER ".\\DATA\\"
#define RPT_FOLDER  ".\\RPT\\"
#define ERR_FOLDER  ".\\ERR\\"

#define MAT_DAT     ".\\DATA\\maticna.dat"
#define TRAN_DAT    ".\\DATA\\transakciona.dat"
#define MAT_NOVA    ".\\DATA\\maticna_nova.dat"

#ifdef __cplusplus

extern "C" {
#endif

	typedef struct {
		unsigned Id;
		int UkupnaPromena;
	} SUM_TRAN;


	typedef struct {
		PROIZVOD* items;
		size_t size;
		size_t capacity;
	} VektorProizvod;

	void vp_init(VektorProizvod* v, size_t initialCapacity);
	void vp_free(VektorProizvod* v);
	int vp_push(VektorProizvod* v, const PROIZVOD* p);
	int vp_insert_at(VektorProizvod* v, size_t index, const PROIZVOD* p);
	int vp_find_by_id(const VektorProizvod* v, unsigned id);
	PROIZVOD* vp_get(VektorProizvod* v, size_t index);
	int vp_remove_by_index(VektorProizvod* v, size_t index);

	typedef struct {
		TRANSAKCIJA* items;
		size_t size;
		size_t capacity;
	} VektorTransakcija;

	void vt_init(VektorTransakcija* v, size_t initialCapacity);
	void vt_free(VektorTransakcija* v);
	int vt_push(VektorTransakcija* v, const TRANSAKCIJA* t);
	int vt_find_by_id(const VektorTransakcija* v, unsigned id);
	TRANSAKCIJA* vt_get(VektorTransakcija* v, size_t index);
	int vt_remove_by_index(VektorTransakcija* v, size_t index);

	int dat_ucitaj_maticnu(const char* path, VektorProizvod* out);
	int dat_sacuvaj_maticnu(const char* path, const VektorProizvod* v);
	int dat_ucitaj_transakcije(const char* path, VektorTransakcija* out);

	void formirajDatum(char* out);
	int kopirajFajl(const char* src, const char* dest);
	int obaveznoKopirajAkoNePostoji(const char* cilj, const char* izvor);
	int uporediTransakcije(const void* a, const void* b);

	typedef void (*MenuAction)(void);

	typedef struct {
		char* text;
		MenuAction action;
	} MenuItem;

	typedef struct {
		char* title;
		MenuItem* items;
		size_t count;
		size_t capacity;
	} Menu;

	void menu_init(Menu* m, const char* title);
	void menu_free(Menu* m);
	int menu_add_item(Menu* m, const char* label, MenuAction action);
	void menu_run(Menu* m);

	void ispisiZaglavlje(void);
	void adts_run_main_menu(void);
	void adts_set_update_all_action(MenuAction action);

#ifdef __cplusplus
}
#endif

#endif