#include "Connect4CV.h"
#include "lodepng.h"
#include <iostream>
#include <fstream>

typedef struct RgbColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} RgbColor;

typedef struct HsvColor {
	unsigned char h;
	unsigned char s;
	unsigned char v;
} HsvColor;

RgbColor HsvToRgb(HsvColor hsv) {
	RgbColor rgb;
	unsigned char region, remainder, p, q, t;

	if (hsv.s == 0) {
		rgb.r = hsv.v;
		rgb.g = hsv.v;
		rgb.b = hsv.v;
		return rgb;
	}

	region = hsv.h / 43;
	remainder = (hsv.h - (region * 43)) * 6;

	p = (hsv.v * (255 - hsv.s)) >> 8;
	q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
	t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

	switch (region) {
	case 0:
		rgb.r = hsv.v; rgb.g = t; rgb.b = p;
		break;
	case 1:
		rgb.r = q; rgb.g = hsv.v; rgb.b = p;
		break;
	case 2:
		rgb.r = p; rgb.g = hsv.v; rgb.b = t;
		break;
	case 3:
		rgb.r = p; rgb.g = q; rgb.b = hsv.v;
		break;
	case 4:
		rgb.r = t; rgb.g = p; rgb.b = hsv.v;
		break;
	default:
		rgb.r = hsv.v; rgb.g = p; rgb.b = q;
		break;
	}

	return rgb;
}

HsvColor RgbToHsv(RgbColor rgb) {
	HsvColor hsv;
	unsigned char rgbMin, rgbMax;

	rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
	rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

	hsv.v = rgbMax;
	if (hsv.v == 0) {
		hsv.h = 0;
		hsv.s = 0;
		return hsv;
	}

	hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
	if (hsv.s == 0) {
		hsv.h = 0;
		return hsv;
	}

	if (rgbMax == rgb.r)
		hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
	else if (rgbMax == rgb.g)
		hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
	else
		hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

	return hsv;
}


Connect4CV::Connect4CV() {
	std::fstream configFile(configPath, std::ios_base::in);
	if (!configFile.is_open()) {
		std::cout << "\nConfig file ("<< configPath <<") not found!\n";
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 2; j++) {
			configFile >> points[i][j];
		}
	}

	configFile >> hueXmin;
	configFile >> hueXmax;
	configFile >> hueOmin;
	configFile >> hueOmax;
	configFile >> saturationThreshold;
	configFile >> cellSize;
	configFile >> threshold;

}


Connect4CV::~Connect4CV() {
}

void Connect4CV::analyse(Igra & igra) {
	loadImageData(inputPath);

	for (int i = 0; i < igra.h; i++) {
		int x2 = (points[0][0] * (igra.h - i - 1) + points[1][0] * i) / (igra.h - 1);
		int y2 = (points[0][1] * (igra.h - i - 1) + points[1][1] * i) / (igra.h - 1);

		int x1 = (points[3][0] * (igra.h - i - 1) + points[2][0] * i) / (igra.h - 1);
		int y1 = (points[3][1] * (igra.h - i - 1) + points[2][1] * i) / (igra.h - 1);

		for (int j = 0; j < igra.w; j++) {
			int x = (x1*(igra.w - j - 1) + x2 * j) / (igra.w - 1);
			int y = (y1*(igra.w - j - 1) + y2 * j) / (igra.w - 1);

			int nX = 0;
			int nO = 0;
			int n = 0;

			for (int px = x - cellSize; px < x + cellSize; px++) {
				for (int py = y - cellSize; py < y + cellSize; py++) {
					if (px >= 0 && px < width && py >= 0 && py < height) {
						int k = (px + py * width) * 4;

						RgbColor rgb;
						HsvColor hsv;

						rgb.r = imageData[k + 0];
						rgb.g = imageData[k + 1];
						rgb.b = imageData[k + 2];

						hsv = RgbToHsv(rgb);

						n++;
						imageData[k + 0] = 200;
						imageData[k + 1] = 200;
						imageData[k + 2] = 200;
						if (hsv.s > saturationThreshold) {
							if (hueXmin > hueXmax) { // yellow
								if (hueXmin < hsv.h || hsv.h < hueXmax) {
									nX++;
									imageData[k + 0] = 255;
									imageData[k + 1] = 215;
									imageData[k + 2] = 0;
								}
							}
							else {
								if (hueXmin < hsv.h && hsv.h < hueXmax) {
									nX++;
									imageData[k + 0] = 255;
									imageData[k + 1] = 215;
									imageData[k + 2] = 0;
								}
							}
							if(hueOmin > hueOmax) { // green
								if (hueOmin < hsv.h || hsv.h < hueOmax) {
									nO++;
									imageData[k + 0] = 127;
									imageData[k + 1] = 204;
									imageData[k + 2] = 69;
								}
							}
							else {
								if (hueOmin < hsv.h && hsv.h < hueOmax) {
									nO++;
									imageData[k + 0] = 127;
									imageData[k + 1] = 204;
									imageData[k + 2] = 69;
								}
							}
						}
						else {
							imageData[k + 0] = 255;
							imageData[k + 1] = 255;
							imageData[k + 2] = 255;
						}

						
					}
				}
				
				for (int px = x - 5; px < x + 5; px++) {
					for (int py = y - 5; py < y + 5; py++) {
						if (px >= 0 && px < width && py >= 0 && py < height) {
							int k = (px + py * width) * 4;

							imageData[k + 0] = 0;
							imageData[k + 1] = 0;
							imageData[k + 2] = 0;

							if (nX > n * threshold) {
								imageData[k + 0] = 255;
								imageData[k + 1] = 255;
								imageData[k + 2] = 0;
							}

							if (nO > n * threshold) {
								imageData[k + 0] = 0;
								imageData[k + 1] = 255;
								imageData[k + 2] = 0;
							}

						}
					}
				}

				igra.data[i][j] = 0;
				if (nX > n*threshold) {
					igra.data[i][j] = Igra::X;
				}
				if (nO > n*threshold) {
					igra.data[i][j] = Igra::O;
				}

			}
		}
	}
	//igra.prikazi();


	saveImageData("analysed.png");
}

void Connect4CV::loadImageData(std::string inputPath) {
	std::vector<unsigned char> inputFile;
	unsigned error = lodepng::load_file(inputFile, inputPath);
	if (!error) error = lodepng::decode(imageData, width, height, inputFile);
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

void Connect4CV::saveImageData(std::string outputPath) {
	std::vector<unsigned char> outputFile;
	unsigned error = lodepng::encode(outputFile, imageData, width, height);
	if (!error) lodepng::save_file(outputFile, outputPath);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}
