#pragma once
#include "Igra.h"
#include "Moznost.h"
class Minimax {
public:
	int globina, nakljucnaIzbira;

	Minimax(int globinaparam);
	~Minimax();

	Moznost odlocitev(Igra igra);
	Moznost rekurzivnoDrevo(Igra igra, int n);
};

