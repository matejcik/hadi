#include <had.hpp>

unsigned long Random::seed = 1;

unsigned long Random::get ()
{
	seed = seed * 1103515245 + 12345;
	return seed;
} // opsano odkudsi z manualu...
