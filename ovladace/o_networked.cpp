#include <had.hpp>
#include <network.hpp>
#include "ovladace/o_networked.hpp"

//konstruktor
TNetworkedOvl::TNetworkedOvl (Hadovnik h, Uint8 id) {
	h->novy_ovladac (this);
	klient = id;
};

//vraci true, kdyz je neco ve fronte, jinak false
bool TNetworkedOvl::ready () {
	return (fronta.size() > 0);
}


//get() je zavolan zvenku, odebere ze zacatku frony smer a vraci ho
Smer TNetworkedOvl::get () {
	Smer smer;
	if ( ready() ) smer = fronta.front();	//ulozi si prvni smer na rade
		else throw(3);	
	fronta.pop_front(); // vymaze jej
	return (smer);	//a vrati
}

void TNetworkedOvl::command(Smer smer) {
	fronta.push_back(smer);
}

//vraci 1.prvek fronty a posle ho na sit (nemaze ho) pomoci globalni network
void TNetworkedOvl::set() {
	Smer smer;
	if ( ready() ) smer = fronta.front();	//ulozi si prvni smer na rade
		else throw(3);	
	network.send(klient, smer);
}

