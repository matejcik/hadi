#include <had.hpp>

int THad::lastid = 0;

THad::THad () : mrtev (true), rychlost (50)
{
	id = lastid;
	lastid++;
	inventar = new TInventar;
}

THad::~THad () { }

void THad::spawn (Coords c, Smer s)
{
	TFoodBonus* jidlo = new TFoodBonus(3); // tady je zakladni delka. todo: nejak to oddelit
	snez(jidlo);
	jidlo->eat();

	mrtev = false;
	telo.push_back (c);
	(*mapa)(c) = this;
	kamjdu = s;

	objeveni (this);
	zmena_smeru (this, s);
	pohyb (this, c);
}

void THad::jdi_na (Coords c)
{
	telo.push_back (c);
	(*mapa)(c) = this;
	if (! inventar->jidlo()) { // pokud uz nemame co jist, hybeme ocasem
		(*mapa)(telo.front()) = NULL;
		telo.pop_front ();
	}

	pohyb (this, c);
}

void THad::ted_jdes (Smer s)
{
	if (s != kamjdu) {
		kamjdu = s;
		zmena_smeru (this,s);
	}
}

Smer THad::kam_jdes () const
{
	return kamjdu;
}

Rychlost THad::jak_rychle () const
{
	int i = rychlost - inventar->rychlost();
	return (i>HAD_SPEED_LIMIT)?i:HAD_SPEED_LIMIT;
}

void THad::snez (Bonus b)
{
	inventar->interact (b);
	snedeni (this,b);
}

Coords THad::hlava () const
{
	if (! telo.empty()) return telo.back();
	else throw (1);
}

void THad::umri ()
{
	for (Telo::iterator i = telo.begin() ; i != telo.end() ; i++) (*mapa)(*i) = NULL;
	// vymazani se z mapy...
	telo.clear();
	mrtev = true;
	inventar->kill ();

	smrt (this);
}

bool THad::jsi_mrtev () const
{
	return mrtev;
}

void THad::interact (Had h, Coords c)
{
	h->umri (); // pokud do me narazi had h, zemrel
	if (!mrtev && c == hlava ()) umri ();
	// pokud ja jeste ziju (tj. nezemrel jsem v predchozim kroku) a narazil mi do hlavy, umiram take
}
