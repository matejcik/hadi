#ifndef HAD_STDOUT__HPP
#define HAD_STDOUT__HPP

#include <had.hpp>

class TStdoutHad : public THad {
	public:
		typedef TTempSpawner<TStdoutHad> spawner;
		
		virtual void spawn (Coords, Smer);
		virtual void umri ();
		virtual void jdi_na (Coords);
		virtual void snez (Bonus);
};

#endif
