#ifndef OVL_NETWORKED__HPP
#define OVL_NETWORKED__HPP

#include <had.hpp>
#include <SDL/SDL_net.h>

class TNetworkedOvl : public TOvladac {
	public:
		TNetworkedOvl (Hadovnik, Uint8);
		
		virtual bool ready ();	//zda je neco ve fronte
		virtual Smer get (); //vraci prvni smer z fronty a maze ho
		virtual void command(Smer); //prida na konec fronty dalsi smer
		virtual void set (); 	//vraci 1.prvek fronty a posle ho na sit (nemaze ho) pomoci network.send
	protected: 
		Uint8 klient;
		typedef std::list<Smer> Fronta_smeru;
		Fronta_smeru fronta;

};
typedef TNetworkedOvl* NetworkedOvl;
#endif
