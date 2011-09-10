#include <had.hpp>

TInventar::TInventar () { }
TInventar::~TInventar () { }

void TInventar::vyrad (Bonus b)
{
	for (Batoh::iterator i = inventar.begin (); i != inventar.end (); i++) if ((*i) == b) {
		inventar.erase (i);
		return;
	}
}

void TInventar::interact (Bonus b)
{
	for (Batoh::iterator i = inventar.begin(); i != inventar.end(); i++) 
		if (! b->interact (*i)) {
			b->kill ();
			return; // pokud se predmet interakci "vycerpal", vrati interact() false..
		}
	
	inventar.push_back (b); // ...jinak je zarazen do inventare
	b->spotrebovan.connect (this,&TInventar::vyrad);
}

bool TInventar::jidlo ()
{
	for (Batoh::iterator i = inventar.begin() ; i != inventar.end(); i++)
		if ((*i)->jsi_typu (TB_JIDLO)) {
			static_cast<TFoodBonus*>(*i)->consume ();
			return true;
		}

	return false;
}

Rychlost TInventar::rychlost ()
{
	Rychlost r = 0;
	for (Batoh::iterator i = inventar.begin() ; i != inventar.end(); i++)
		if ((*i)->jsi_typu (TB_RYCHLOST)) r += static_cast<TSpeedBonus*>(*i)->rychlost ();

	return r;
}

void TInventar::kill ()
{
	Bonus b;
	while (!inventar.empty()) {
		b = inventar.front ();
		inventar.pop_front ();
		b->kill ();
	}
}
