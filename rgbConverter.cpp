#include "rgbConverter.h"
#include <string>
#include <iostream>

float* rgb_to_hsv(float red, float green, float blue) {
	float* hsvData = new float[3];

	float max = getMax(getMax(red, green), getMax(green, blue));
	float min = getMin(getMin(red, green), getMin(green, blue));
	float mid = red + green + blue - min - max;

	int h = 0;
	if (max != min) {
		if (red == max) {
			h = 60 * (green - blue) / (max - min);
		}
		else if (green == max) {
			h = 60 * (blue - red) / (max - min) + 120;
		}
		else if (blue == max) {
			h = 60 * (red - green) / (max - min) + 240;
		}
	}
	if (h < 0) h += 360;

	float s = 0;
	if (max != 0) {
		s = (max - min) / max;
	}

	hsvData[0] = h % 360;
	hsvData[1] = s;
	hsvData[2] = max;

	return hsvData;
}

float* rgb_to_cmy(float r, float g, float b) {
	float* cmyData = new float[3];

	cmyData[0] = 1.0f - r;
	cmyData[1] = 1.0f - g;
	cmyData[2] = 1.0f - b;

	return cmyData;
}

void storeInput(float& rF, float& gF, float& bF) {
	std::string r, g, b;
	bool input_correct = false;
	while (!input_correct) {
		try {
			std::cin >> r;
			rF = std::stof(r);
			if (rF > 1 || rF < 0) {
				std::cout << "Gib einen Wert zwischen 0 und 1 ein" << std::endl;
				continue;
			}
			std::cin >> g;
			gF = std::stof(g);
			if (gF > 1 || gF < 0) {
				std::cout << "Gib einen Wert zwischen 0 und 1 ein" << std::endl;
				continue;
			}
			std::cin >> b;
			bF = std::stof(b);
			if (bF > 1 || bF < 0) {
				std::cout << "Gib einen Wert zwischen 0 und 1 ein" << std::endl;
				continue;
			}
			input_correct = true;
		}
		catch (const std::invalid_argument& e) {
			std::cout << "Gib einen numerischen Wert ein" << std::endl;
		}
		catch (const std::out_of_range& e) {
			std::cout << "Gib einen erlaubten float Wert ein" << std::endl;
		}
	}

}

float getMax(float a, float b) {
	return a > b ? a : b;
}

float getMin(float a, float b) {
	return a < b ? a : b;
}

void executeRGB_Converter(void) {
	float rF = 0; float gF = 0; float bF = 0;
	storeInput(rF, gF, bF);

	float* hsv = rgb_to_hsv(rF, gF, bF);
	float* cmy = rgb_to_cmy(rF, gF, bF);
	std::cout << "H: " << hsv[0] << std::endl;
	std::cout << "S: " << hsv[1] << std::endl;
	std::cout << "V: " << hsv[2] << std::endl << std::endl;

	std::cout << "C: " << cmy[0] << std::endl;
	std::cout << "M: " << cmy[1] << std::endl;
	std::cout << "Y: " << cmy[2] << std::endl;

	delete[] hsv;
	delete[] cmy;
}
