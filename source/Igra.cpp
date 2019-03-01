#include "Igra.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>


Igra::Igra(int wparam, int hparam, int n, int zacne) {
	w = wparam;
	h = hparam;
	vVrsto = n;
	naPotezi = zacne;
	koncana = false;
	zmagovalec = 0;
	data = std::vector<std::vector<int>>(h);
	vrednost = 0;
	for (int i = 0; i < h; i++) {
		data[i] = std::vector<int>(w);
		for (int j = 0; j < w; j++) {
			data[i][j] = 0;
		}
	}
}


Igra::~Igra() {

}

void Igra::prikazi() {
	std::cout << "\n";
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < data[i].size(); j++) {
			if (data[i][j] == 1)std::cout << 'X' << " ";
			else if (data[i][j] == -1)std::cout << 'O' << " ";
			else std::cout << '.' << " ";
		}
		std::cout << "\n";
	}
}

int Igra::prostaPolja() {
	//vrni števolo prostih polj
	int st = 0;
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			if (data[j][i] == 0)st++;
		}
	}
	if (st == 0)koncana = true;
	return st;
}

std::vector<Moznost> Igra::moznosti() {
	//vrni vse možne poteze, ki so trenutno na voljo
	std::vector<Moznost> moznosti;
	for (int i = 0; i < w; i++) {
		for (int j = h - 1; j >= 0; j--) {
			if (data[j][i] == 0) {
				moznosti.push_back(Moznost(i, j));
				break;
			}
		}
	}
	return moznosti;
}

int Igra::ovrednoti() {
	//povej ali je kdo zmagal
	int n = vVrsto; //koliko znakov mora biti v vrsti
	vrednost = 0;
	zmagovalec = 0;
	int smeri[4][2] = {
		{1,0}, // vodoravno
		{0,1}, // navpicno
		{1,1}, // posevno dol
		{-1,1} // posevno gor
	};
	for (int s = 0; s < 4; s++) {
		int smerX = smeri[s][0];
		int smerY = smeri[s][1];
		for (int j = 0-std::min(smerY,0)*(n-1); j < h-std::max(smerY,0)*(n-1); j++) {
			for (int i = 0-std::min(smerX,0)*(n-1); i < w-std::max(smerX,0)*(n-1); i++) {
				//prestej koliko znakov je v vrsti
				int vrsta[3] = { 0,0,0 }; // st zetonov igralca -1, st praznih mest, st zetonov igralca +1
				for (int k = 0; k < n; k++) {
					vrsta[1 + poglej(i + smerX * k, j + smerY * k)]++;
				}
				if (vrsta[2] == 0)vrednost -= pow(100, vrsta[0]);
				if (vrsta[0] == 0)vrednost += pow(100, vrsta[2]);
				if (vrsta[2] == n)zmagovalec = +1;
				if (vrsta[0] == n)zmagovalec = -1;
			}
		}
	}
	if (zmagovalec != 0) {
		koncana = true;
	}
	return zmagovalec;
}

int Igra::poglej(int x, int y) {
	if (x >= 0 && x < w && y >= 0 && y < h) {
		return data[y][x];
	}
	return 0;
}

bool Igra::postavi(int x, int y) {
	//če je igra končana ne postavi
	if (koncana)return false;
	//če ni na igralni površini ne postavi
	if (x < 0 || y < 0 || x >= w || y >= h)return false;
	//če ni prosto polje ne postavi
	if (data[y][x] != 0)return false;

	data[y][x] = naPotezi;
	if (naPotezi == 1)naPotezi = -1;
	else naPotezi = 1;

	//ovrednoti igro (nastavi koncana in zmagovalec)
	ovrednoti();
	prostaPolja();

	return true;
}

bool Igra::postaviVStolpec(int stolpec) {
	auto m = moznosti();
	for (int i = 0; i < m.size(); i++) {
		if (m[i].x == stolpec) {
			postavi(m[i].x, m[i].y);
			return true;
		}
	}
	return false;
}

int Igra::visinaStolpca(int stolpec) {
	auto m = moznosti();
	for (int i = 0; i < m.size(); i++) {
		if (m[i].x == stolpec) {
			return m[i].y;
		}
	}
	return -1;
}

Igra Igra::copy() {
	Igra igra(w, h, vVrsto, naPotezi);
	igra.data = data;
	return igra;
}
