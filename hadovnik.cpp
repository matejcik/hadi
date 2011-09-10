#include <had.hpp>

THadovnik::THadovnik ()
{
	spawn = new THad::spawner;
	vyhodnocovac = new TVyhodnocovac;
}

THadovnik::THadovnik (Spawner s)
{
	spawn = s;
	vyhodnocovac = new TVyhodnocovac;
}

THadovnik::~THadovnik ()
{
	delete vyhodnocovac;
}

void THadovnik::set_spawner (Spawner s)
{
	delete spawn;
	spawn = s;
}

void THadovnik::set_map (Mapa m)
{
	mapa = m;
	vyhodnocovac->set_map(m);
	for (HadOvls::iterator i = hadi.begin() ; i != hadi.end() ; i++)
		(*i).had->set_map(m);
}

#define SPEEDSTEP 10
// XXX to taky jinam
void THadovnik::tick ()
{
	for (std::vector<HadOvl>::iterator i = hadi.begin(); i != hadi.end(); i++) {
		i->remain += SPEEDSTEP;
		Rychlost r = i->had->jak_rychle ();
		while (i->ovladac->ready() && (i->remain > r) && (r > 0)) {
			i->remain -= r;
			Smer os = i->ovladac->get();
			vyhodnocovac->proved (os, i->had);
			r = i->had->jak_rychle ();
		}
	}
}

Had THadovnik::novy_ovladac (Ovladac o)
{
	HadOvl h;
	h.had = (*spawn)(); // k tomuhle se uziva spawner. je to vlastne new ZvolenyTypHada.
	h.had->set_map (mapa);
	h.ovladac = o;
	h.remain = 0;
	hadi.push_back (h);

	return h.had;
}
