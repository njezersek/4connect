#include <iostream>
#include "Igra.h"
#include "Minimax.h"
#include "Connect4CV.h"
using namespace std;

int main() {
	Igra igra(7, 6, 4, Igra::X);
	Minimax algoritem(5);

	Connect4CV cv;

	system("python captureImage.py");

	cv.analyse(igra);

	igra.prikazi();

	Moznost odlocitev = algoritem.odlocitev(igra);
	std::cout << odlocitev.x << " " << odlocitev.y;

	std::cout << "Done";

	cin.get();
	return 0;
}
