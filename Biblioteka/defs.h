#ifndef DEFS_H
#define DEFS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NAZIV_LEN 64
#define ULAZ  1
#define IZLAZ -1

	typedef struct {
		unsigned Id;
		char Naziv[MAX_NAZIV_LEN];
		unsigned Kolicina;
	} PROIZVOD;

	typedef struct {
		unsigned Id;
		int Promena;
		unsigned Kolicina;
	} TRANSAKCIJA;

#ifdef __cplusplus
}
#endif

#endif