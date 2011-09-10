#include <had.hpp>

#include "o_brandom.hpp"

TBetterRandomOvl::TBetterRandomOvl (Hadovnik h)
{
	had = h->novy_ovladac (this);
	mapa = h->get_map ();
}

Smer TBetterRandomOvl::get ()
{
	Smer smer;
	if (had->jsi_mrtev()) return (Smer)0;

	switch (Random::get() % 6) {
		case 0: smer = H_XPLUS; break;
		case 1: smer = H_XMINUS; break;
		case 2: smer = H_YPLUS; break;
		case 3: smer = H_YMINUS; break;
		case 4: smer = H_ZPLUS; break;
		case 5: smer = H_ZMINUS; break;
	}

	if (-smer == had->kam_jdes()) smer = static_cast<Smer>(-smer);

	Coords c = had->hlava();

	Smer t = smer;
	while (true) {
		if ((*mapa)(c(smer)) <= M_OUTSIDE) break;
		if ((*mapa)(c(smer))->jsi_typu(T_BONUS)) break;
		smer = static_cast<Smer>(smer + 1);
		if (!smer) smer = H_XPLUS;
		if (-smer == had->kam_jdes()) smer = static_cast<Smer>(smer + 1);
		if (smer > H_ZPLUS) smer = H_ZMINUS;
		if (smer == t) break;
	}

	return smer;
}
