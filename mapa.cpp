#include <had.hpp>

TMapa::TMapa (int x, int y, int z) : rozmery(x,y,z), outside(M_OUTSIDE)
{
	// vytvareni mapy z vektoru vektoru vektoru
	Line l (z);
	Surface s (y, l);

	mapa.clear ();
	mapa.reserve (x);
	for (int i = 0; i < x; i++) mapa.push_back (s);
	// jak rychle je tohle ?
	
	spawnpoints.reserve (5);
	spawnpoints.push_back (Coords(1,1,1));
	spawnpoints.push_back (Coords(3,1,3));
	spawnpoints.push_back (Coords(5,1,5));
	spawnpoints.push_back (Coords(7,1,7));
	spawnpoints.push_back (Coords(9,1,9));
	// fixni spawnpointy, budou se nacitat z mapoveho souboru
	// y = 1 aby se to dalo hrat (nemame pohyb po y)
}

Vec& TMapa::operator() (int x, int y, int z)
{
	if (x >= 0 && x < rozmery.x && y >= 0 && y < rozmery.y && z >= 0 && z < rozmery.z)
		return mapa[x][y][z];
	else return outside;
}

Vec& TMapa::operator() (const Coords& c)
{
	if (c.x >= 0 && c.x < rozmery.x && c.y >= 0 && c.y < rozmery.y && c.z >= 0 && c.z < rozmery.z)
		return mapa[c.x][c.y][c.z];
	else return outside;
}

Coords TMapa::get_spawnpoint ()
{
	if (spawnpoints.empty()) throw (1);

	int i = Random::get() % spawnpoints.size();

	// kontrola, jestli na tom spawnpointu nekdo neni.
	int j(i);
	while ((*this)(spawnpoints[i])) {
		i++;
		if (i>=spawnpoints.size()) i = 0; // wrap-around
		if (i == j) break; // prochazime jen jednou
	}
	
	if ((*this)(spawnpoints[i])) throw (2); // vsechny spawnpointy obsazene, co budeme delat o my ubozi?
	return spawnpoints[i];
}

Coords TMapa::get_free ()
{
	Coords ret;
	ret.x = Random::get() % rozmery.x;
	ret.y = Random::get() % rozmery.y;
	ret.z = Random::get() % rozmery.z;

	Coords tmp = ret;
	while ((*this)(ret)) { // prochazime postupne uplne vsechny body, v danem poradi, dokud nenajdeme volny
		ret.x++;
		if (ret.x >= rozmery.x) ret.x = 0;
		if (ret.x == tmp.x) ret.y++;
		if (ret.y >= rozmery.y) ret.y = 0;
		if (ret.x == tmp.x && ret.y == tmp.y) ret.z++;
		if (ret.z >= rozmery.z) ret.z = 0;
		if (ret.x == tmp.x && ret.y == tmp.y && ret.z == tmp.z) break;
	}
	return ret;
	// ...a pokud jsme prece jen volny nenasli, tak mame smulu. proste to neco prepise.
	// nasledky budou katastrofalni, takze XXX tohle nejak vychytat
}
