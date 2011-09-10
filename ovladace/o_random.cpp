#include <had.hpp>

#include "o_random.hpp"

TRandomOvl::TRandomOvl (Hadovnik h)
{
	h->novy_ovladac (this);
}

Smer TRandomOvl::get ()
{
	switch (Random::get() % 6) {
		case 0: return H_XPLUS;
		case 1: return H_XMINUS;
		case 2: return H_YPLUS;
		case 3: return H_YMINUS;
		case 4: return H_ZPLUS;
		case 5: return H_ZMINUS;
	}
}
