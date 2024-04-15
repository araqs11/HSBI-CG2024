#pragma once
#include <string>

float* rgb_to_hsv(float, float, float);
float* rgb_to_cmy(float, float, float);
float getMax(float, float);
float getMin(float, float);
void storeInput(float&, float&, float&);
void executeRGB_Converter(void);
