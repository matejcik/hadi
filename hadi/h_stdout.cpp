#include <had.hpp>
#include <iostream>

#include "h_stdout.hpp"

void TStdoutHad::spawn (Coords c, Smer s)
{
	std::cout << "had " << id << " se objevil na ";
	std::cout << '(' << c.x << ',' << c.y << ',' << c.z << ")";
	std::cout << std::endl;
	THad::spawn(c,s);
}
	

void TStdoutHad::jdi_na (Coords c)
{
	THad::jdi_na (c);

	
	std::cout << "had " << id << " jde na ";
	for (Telo::reverse_iterator i = telo.rbegin(); i != telo.rend(); i++)
		std::cout << '(' << i->x << ',' << i->y << ',' << i->z << ") ";
	std::cout << std::endl;

}

void TStdoutHad::umri ()
{
	std::cout << "had " << id << " je mrtev!" << std::endl;
	THad::umri();
}

void TStdoutHad::snez (Bonus b)
{
	std::cout << "had " << id << " neco snedl!" << std::endl;
	THad::snez(b);
}
