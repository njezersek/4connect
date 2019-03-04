#include <iostream>
#include <string>
#include "Igra.h"
#include "Minimax.h"
#include "Connect4CV.h"
#include "Bot.h"
using namespace std;

int tezavnost = 4;
int zacne = Igra::X;
int igralec = Igra::X;

Bot bot;

void clearConsole() {
#ifdef _WIN32
	system("cls");
#elif __linux__
	system("clear");
#endif
}

void captureImage() {
#ifdef _WIN32
	
#elif __linux__
	system("raspistill -e png -o capture.png --nopreview -t 1 -w 400 -h 300");
#endif
}


void izberiTezavnost() {
	int izbira = -1;
	while (izbira == -1) {
		std::cout << "\nIzberi tezavnost [0-9]\n:";
		std::cin >> izbira;
		if (izbira < 0 || izbira > 9)izbira = -1;
	}
	tezavnost = izbira;
}

void izberiZacne() {
	char izbira = '-';
	while (izbira == '-') {
		std::cout << "\nIzberi kdo zacne [X/O]\n:";
		std::cin >> izbira;
		if (izbira == 'X' || izbira == 'x')zacne = Igra::X;
		else if (izbira == 'O' || izbira == 'o')zacne = Igra::O;
		else izbira = '-';
	}
}

void izberiSimbolIgralca() {
	char izbira = '-';
	while (izbira == '-') {
		std::cout << "\nIzberi simbol igralca [X/O]\n:";
		std::cin >> izbira;
		if (izbira == 'X' || izbira == 'x')igralec = Igra::X;
		else if (izbira == 'O' || izbira == 'o')igralec = Igra::O;
		else izbira = '-';
	}
}

void zaciniIgro() {
	clearConsole();
	bot.goHome();
	std::cout << "=== Nova igra ===\ntezavnost: " << tezavnost << "\n";
	Connect4CV cv;
	cv.debug = true;
	cv.player = igralec;
	Igra igra(7, 6, 4, zacne);
	Minimax algoritem(tezavnost);

	while (!igra.koncana) {
		if (igra.naPotezi == igralec) {
			std::cout << "\n-- Na potezi je igralec (" << ((igralec == Igra::X) ? 'X' : 'O') << ") -- \n";
			std::cout << "Vrzi zeton in pritisni enter ...\n";
			char izbira = -1;
			izbira = std::cin.get();
			if (izbira == 'q')break;
			std::cout << "Zajemam sliko ...\n";
			captureImage();
			std::cout << "Analiziram sliko ...\n";
			igra.naPotezi *= -1;
			cv.analyse(igra);
			igra.prikazi();
		}
		else {
			std::cout << "\n-- Na potezi je racunalnik (" << ((igralec*-1 == Igra::X) ? 'X' : 'O') << ") -- \n";
			std::cout << "Odlocam se za potezo ...\n";
			Moznost odlocitev = algoritem.odlocitev(igra);
			std::cout << "Vrgel sem v stolpec " << odlocitev.x << "\n";
			bot.dropColumn(odlocitev.x);
			igra.postavi(odlocitev.x, odlocitev.y);
			igra.prikazi();
		}

		igra.ovrednoti();
		if (igra.koncana) {
			std::cout << "\n# konec igre\n";
			std::cout << "Zmagal je " << (igra.zmagovalec == igralec ? "igralec" : "racunalnik") << "(" << ((igra.zmagovalec == Igra::X) ? 'X' : 'O') << ")\n";
		}
	}
	std::cout << "Pritisni enter za vrnitev v glavni menu.";
	std::cin.get();
}

void testnaSlika() {
	clearConsole();
	std::cout << "Test analize slike\n";
	Connect4CV cv;
	cv.player = igralec;
	Igra igra(7, 6, 4, zacne);
	cv.debug = true;
	captureImage();
	std::cout << "Zajeta tesnta slika in shranjena v \"capture.png\".\n";
	cv.analyse(igra);
	std::cout << "Alaizirana slika shranjena v \"analysed.png\".\n";
	std::cout << "Rezultat analize:\n";
	igra.prikazi();
	std::cout << "\n---\nPritisni enter za vrnitev v glavni menu.\n";
	std::cin.get();
}


void menu() {
	/* main menu */
	int izbira = -1;
	while (izbira == -1) {
		clearConsole();
		std::cout << "\n=== Connect4 bot ===\n";
		std::cout << "[1] zacni igro\n";
		std::cout << "[2] tezavnost: "<< tezavnost << "\n";
		std::cout << "[3] zacne igralec: "<< ((zacne == Igra::X) ? 'X' : 'O')  << "\n";
		std::cout << "[4] simbol igralca: " << ((igralec == Igra::X) ? 'X' : 'O') << "\n";
		std::cout << "[5] test analize slike\n";
		std::cout << "[0] zapri program\n";
		std::cout << "\n:";
		std::cin >> izbira;
		std::cin.ignore(1000, '\n');
		if (izbira < 0 || izbira > 5)izbira = -1;
		if (izbira == 1) zaciniIgro();
		if (izbira == 2) izberiTezavnost();
		if (izbira == 3) izberiZacne();
		if (izbira == 4) izberiSimbolIgralca();
		if (izbira == 5) testnaSlika();
		if (izbira != 0) izbira = -1;
	}
}

int main() {
	Igra igra(7, 6, 4, Igra::X);
	Minimax algoritem(5);

	Connect4CV cv;

	menu();
	return 0;
}
