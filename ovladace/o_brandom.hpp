#ifndef OVL_BRANDOM__HPP
#define OVL_BRANDOM__HPP

#include <had.hpp>

class TBetterRandomOvl : public TOvladac {
	public:
		TBetterRandomOvl (Hadovnik);
		virtual Smer get ();
		Had had;
	protected:
		Mapa mapa;
};

#endif
