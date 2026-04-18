#ifndef MISC_H
#define MISC_H

#include "defs.h"
#include "adts.h"

#ifdef __cplusplus
extern "C" {
#endif
	void kreirajMaticnuDatoteku(void);
	void obrisiMaticnuDatoteku(void);
	void ubaciProizvod(void);
	void azurirajJedanProizvod(void);
	void prikaziSveProizvode(void);
	void prikaziJedanProizvod(void);
	void obrisiProizvod(void);

	void procesuirajSlucaj1(void);
	void procesuirajSlucaj2(void);
	void procesuirajSlucaj3(void);
	void procesuirajSlucaj4(void);
	void procesuirajSlucaj5(void);

	void kreirajTransakcionuDatoteku(void);
	void obrisiTransakcionuDatoteku(void);
	void ubaciTransakciju(void);
	void prikaziSveTransakcije(void);
	void prikaziTransakcijeJednogProizvoda(void);

#ifdef __cplusplus
}
#endif

#endif