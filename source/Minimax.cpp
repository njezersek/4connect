#include "Minimax.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>
#include <string>
#include <limits>


Minimax::Minimax(int globinaparam) {
	globina = globinaparam;
	nakljucnaIzbira = 1;

	srand(time(NULL));
}


Minimax::~Minimax() {
}

Moznost Minimax::odlocitev(Igra igra) {
	return rekurzivnoDrevo(igra, globina);
}

Moznost Minimax::rekurzivnoDrevo(Igra igra, int n) {
	std::vector<Moznost> moznosti = igra.moznosti();

	for (int l = 0; l < moznosti.size(); l++) {
		Moznost& moznost = moznosti[l];

		//ovrednoti moznost
		Igra novaIgra = igra.copy();
		novaIgra.postavi(moznost.x, moznost.y);
		//novaIgra.prikazi();
		//std::cout << "moznost: (x: " << moznost.x << ", y: " << moznost.y << ") " ;
		int zmaga = novaIgra.zmagovalec;
		double vrednost = novaIgra.vrednost;

		//nastavi moznost
		moznost.oddaljenost = globina - n;
		moznost.zmaga = zmaga;
		moznost.vrednost = vrednost;

		//std::cout << "vrednost" << moznost.vrednost << "\n";

		//če ni nihče zmagal in nisi še presegel globine
		//in je še kakšno prosto polje, lahko se zgodi da so vsa polja polna in
		//je rezultat izenačeno potem pride error
		if (zmaga == 0 && novaIgra.prostaPolja() > 0 && n > 0) {
			Moznost ovrednotenaMoznost = rekurzivnoDrevo(novaIgra, n - 1);

			moznost.zmaga = ovrednotenaMoznost.zmaga;
			moznost.vrednost = ovrednotenaMoznost.vrednost;
			moznost.oddaljenost = ovrednotenaMoznost.oddaljenost;
		}
	}

	/*if (true) {
		igra.prikazi();
		std::cout << n << "-----\n";
		std::string out1 = " * ";
		std::string out2 = " * ";
		for (int index = 0; index < moznosti.size(); index++) {
			std::cout << "x: " << moznosti[index].x << " z: " << moznosti[index].zmaga << " v: " << moznosti[index].vrednost << ":\n";
		}
		//std::cout << "\n " << out1 << "\n " << out2 << "\n";

	}*/

	//izberi moznosti
	double vmax = -std::numeric_limits<double>::max();
	double vmin = std::numeric_limits<double>::max();
	int imax = -1;
	int imin = -1;
	for (int i = 0; i < moznosti.size(); i++) {
		Moznost m = moznosti[i];
		//na konec dodana stevka, ki zagotovi naključno izbiro poteze
		double v = 10 * m.vrednost /** (globina - m.oddaljenost+1) */+ (rand() % 10) * nakljucnaIzbira;
		//std::cout << m.vrednost* (globina - m.oddaljenost +1) << "\n";
		//poišči min in max med ovrednotenimi možnostmi (veje)
		if (v > vmax) {
			vmax = v;
			imax = i;
		}
		if (v < vmin) {
			vmin = v;
			imin = i;
		}
		//std::cout << "v: " << v << "\n";
	}
	//če je na motezi nasprotnik išči min drugače pa max
	int index = igra.naPotezi == 1 ? imax : imin;
	//std::cout << "na potezi: "<< igra.naPotezi << "index: "<< index << "vrednost: "<< moznosti[index].vrednost << "\n";

	return moznosti[index];
}
