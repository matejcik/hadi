#include <had.hpp>
#include <interface.hpp>

Smer TInterfaceOvl::get ()
{
	return smer;
}

void TInterfaceOvl::keydown (IfSmer s)
{
	next = s;
	keypressed = true;
}

void TInterfaceOvl::keyup (IfSmer s) { }

void TInterfaceOvl::set ()
{
	if (keypressed) {
		keypressed = false;
		Smer s = smer;
		Coords n,r,c;
		n = n(nahoru);
		r = r(smer);
		
		switch (next) {
			case IS_DOWN:
				smer = nahoru;
				nahoru = static_cast<Smer> (-s);
				break;
			case IS_UP:
				smer = static_cast<Smer> (-nahoru);
				nahoru = s;
				break;
			case IS_LEFT: case IS_RIGHT:
				c.x = n.y*r.z - n.z*r.y;
				c.y = n.z*r.x - n.x*r.z;
				c.z = n.x*r.y - n.y*r.x;
				if (c.x) smer = static_cast<Smer> (c.x * H_XPLUS);
				else if (c.y) smer = static_cast<Smer> (c.y * H_YPLUS);
				else smer = static_cast<Smer> (c.z * H_ZPLUS);
				if (next == IS_RIGHT) smer = static_cast<Smer> (-smer);
				break;
		}
		ZmenaSmeru z; z.s = smer; z.i = next; z.n = nahoru;
		zmena_smeru (this, z);
	} 			
}
