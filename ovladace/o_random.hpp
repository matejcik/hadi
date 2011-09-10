#ifndef OVL_RANDOM__HPP
#define OVL_RANDOM__HPP

#include <had.hpp>

class TRandomOvl : public TOvladac {
	public:
		TRandomOvl (Hadovnik);
		virtual Smer get ();
};

#endif
