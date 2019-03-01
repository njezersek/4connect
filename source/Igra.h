#pragma once
#include "Moznost.h"
#include <vector>

class Igra {
public:
	static const int X = 1;
	static const int O = -1;
	int w, h, vVrsto;
	int naPotezi, zmagovalec;
	double vrednost;
	std::vector<std::vector<int>> data;
	bool koncana = false;

	Igra(int wparam, int hparam, int n, int naPotezi);
	~Igra();

	void prikazi();
	int prostaPolja();
	std::vector<Moznost> moznosti();
	int ovrednoti();
	int poglej(int x, int y);
	bool postavi(int x, int y);
	bool postaviVStolpec(int stolpec);
	int visinaStolpca(int stolpec);
	Igra copy();
};

