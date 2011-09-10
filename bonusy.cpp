#include <had.hpp>

void TBonus::interact (Had h, Coords c)
{
	h->snez (this);
	eat ();
}

bool TFoodBonus::interact (Bonus b)
{
	// TODO generalizace tohohle, aby se plnily stejne typy
	if (b->jsi_typu (TB_JIDLO)) { // tzn. nasel jsem jeste nespotrebovany FoodBonus v inventari
		TFoodBonus* fb = static_cast<TFoodBonus*>(b);
		fb->feed (units); // nasypu do nej sve jidlo
		return false; //...a nenecham se zaradit
	} else return true;
}
