#include "colorConverter.h"
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
			
			std::cin >> g;
			gF = std::stof(g);
			
			std::cin >> b;
			bF = std::stof(b);
			
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

void execute_Converter(void) {
	float rF = 0; float gF = 0; float bF = 0;
	float hF = 0; float sF = 0; float vF = 0;

	bool correct = false;
	while (!correct) {
		storeInput(rF, gF, bF);
		if (!(rF < 0 || rF > 1 || gF < 0 || gF > 1 || bF < 0 || bF > 1)) {
			correct = true;
		}
		else {
			std::cout << "Bitte gib Zahlen zwischen 0 und 1 ein" << std::endl;
		}
	}


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
	correct = false;
	while (!correct) {
		storeInput(hF, sF, vF);
		if (!(hF < 0 || hF > 359 || sF < 0 || sF > 1 || vF < 0 || vF > 1)) {
			correct = true;
		}
		else {
			std::cout << "Bitte gib Zahlen zwischen 0 und 1 ein" << std::endl;
		}
	}
	float* hsv_rgb = hsv_to_rgb(hF, sF, vF);
	float* hsv_cmy = hsv_to_cmy(hF, sF, vF);

	std::cout << "HSV Wert als RGB Wert: " << hsv_rgb[0] << ", " << hsv_rgb[1] << ", " << hsv_rgb[2] << std::endl;
	std::cout << "HSV Wert als CMY Wert: " << hsv_cmy[0] << ", " << hsv_cmy[1] << ", " << hsv_cmy[2] << std::endl;

	delete[] hsv_rgb;
	delete[] hsv_cmy;
}

float* hsv_to_rgb(float h, float s, float v) {
	float r, g, b;
	float* values = new float[3];
	float c = v * s;
	float x = c * (1 - std::abs(((int)h / 60) % 2 - 1));
	float m = v - c;

	if (0 <= h && h < 60) { r = c; g = x; b = 0; }
	else if (60 <= h && h < 120) { r = x; g = c; b = 0; }
	else if (120 <= h && h < 180) { r = 0; g = c; b = x; }
	else if (180 <= h && h < 240) { r = 0; g = x; b = c; }
	else if (240 <= h && h < 300) { r = x; g = 0; b = c; }
	else if (300 <= h && h < 360) { r = c; g = 0; b = x; }
	values[0] = (r + m) * 255;
	values[1] = (g + m) * 255;
	values[2] = (b + m) * 255;
	return values;
}

float* hsv_to_cmy(int h, float s, float v) {
	float* rgb = hsv_to_rgb(h, s, v);
	return rgb_to_cmy(rgb[0], rgb[1], rgb[2]);
}