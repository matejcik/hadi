#include <had.hpp>

void TBonusovnik::tick ()
{
	ticker ();
	
	if (! jidlo_v_mape) { // ...pokud ne, dame tam nove
		TFoodBonus* b = new TFoodBonus(1);
		inject (b, mapa->get_free ());
		jidlo_v_mape++;
	}
	
	/*if (!(Random::get () % 50)) {
		TSpeedBonus* s = new TSpeedBonus (500, 10);
		inject (s, mapa->get_free ());
	}*/
}

void TBonusovnik::inject (Bonus b, Coords c)
{
	(*mapa)(c) = b;
	bonusy[b] = c;
	b->sneden.connect (this, &TBonusovnik::snez);
	b->spotrebovan.connect (this, &TBonusovnik::uvolni);
}

void TBonusovnik::inject (TTimedBonus* b, Coords c)
{
	inject ((Bonus)b,c);
	ticker.connect (b, &TTimedBonus::tick);
}

void TBonusovnik::snez (Bonus b)
{
	if (! b->is_eaten()) {
		(*mapa)(bonusy[b]) = NULL;
		if (b->jsi_typu(TB_JIDLO)) jidlo_v_mape--;
		b->sneden.disconnect (this);
	}
}

void TBonusovnik::uvolni (Bonus b)
{
	snez (b);
	bonusy.erase (b);
	b->sneden.disconnect (this);
}
