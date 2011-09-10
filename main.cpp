#include <had.hpp>
#include <interface.hpp>
//#include <network.hpp>
#include "SDL.h"
#include <ctime>
#include "ovladace/o_brandom.hpp"

Interface iface;
// ServerNetwork<TTCPServerSocket,TTCPClientSocket> network;

int main (int argc, char** argv) {
	Random::seed = time(NULL);

	
	iface.init ();
	Interface::Keys k;
	k.left = SDLK_LEFT;
	k.right = SDLK_RIGHT;
	k.up = SDLK_UP;
	k.down = SDLK_DOWN;
	iface.novy_ovladac (k);
	
//	iface.novy_externi_ovl ((TBetterRandomOvl*)(NULL));
//	iface.novy_externi_ovl ((TBetterRandomOvl*)(NULL));
	while (!iface.end()) iface.tick ();	
	
	return 0;
}
