#pragma once
#include <vector>
#include <string>
#include "Igra.h"

class Connect4CV {
public:
	std::string inputPath = "capture.png";
	std::string configPath = "config.txt";
	std::vector<unsigned char> imageData;
	unsigned width, height;
	int points[4][2] = {
		{908, 38},
		{934, 655},
		{74, 689},
		{78, 31}
	};
	int cellSize = 35;
	int saturationThreshold = 0.3 * 255;
	int hueXmin = 0;
	int hueXmax = 0;
	int hueOmin = 0;
	int hueOmax = 0;
	float threshold = 0.333333;

	Connect4CV();
	~Connect4CV();

	void analyse(Igra& igra);

	void loadImageData(std::string inputPath);
	void saveImageData(std::string outputPath);
};

