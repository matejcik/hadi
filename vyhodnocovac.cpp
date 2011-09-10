#include <had.hpp>

void TVyhodnocovac::proved (Smer o, Had h)
{
	if (mapa == NULL) throw (1);

	if (h->jsi_mrtev ()) {
		try { // respawn:
			Coords c = mapa->get_spawnpoint ();
			h->spawn (c, h->kam_jdes ()); //  zatim fixnim smerem
		} catch (...) {
			// no free spawnpoint
		}
	} else {
		Smer smer = h->kam_jdes ();
		Coords c = h->hlava ();
		MapSize s = mapa->get_size();
	
		if (smer == -o) { // celem vzad se nesmi
			h->umri ();
			return;
		}

		switch (o) {
			case H_XPLUS: c.x++; break;
			case H_XMINUS: c.x--; break;
			case H_YPLUS: c.y++; break;
			case H_YMINUS: c.y--; break;
			case H_ZPLUS: c.z++; break;
			case H_ZMINUS: c.z--; break;
		}

		if (c.x >= s.x) c.x = 0;
		if (c.x < 0) c.x = s.x-1;
		if (c.y >= s.y) c.y = 0;
		if (c.y < 0) c.y = s.y-1;
		if (c.z >= s.z) c.z = 0;
		if (c.z < 0) c.z = s.z-1;
		// wrap-around
		
		if ((*mapa)(c)) (*mapa)(c)->interact(h,c);
		h->ted_jdes (o);
		h->jdi_na (c);
	}
}
