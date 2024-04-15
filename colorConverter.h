#pragma once

float* rgb_to_hsv(float, float, float);
float* rgb_to_cmy(float, float, float);
float* hsv_to_rgb(float, float, float);
float* hsv_to_cmy(int, float, float);
float getMax(float, float);
float getMin(float, float);
void storeInput(float&, float&, float&);
void execute_Converter(void);